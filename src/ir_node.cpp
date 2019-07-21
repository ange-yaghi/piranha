#include "../include/ir_node.h"

#include "../include/ir_attribute.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_attribute_definition.h"
#include "../include/compilation_error.h"
#include "../include/ir_attribute_definition_list.h"
#include "../include/ir_value.h"
#include "../include/ir_context_tree.h"
#include "../include/language_rules.h"
#include "../include/node.h"
#include "../include/custom_node.h"
#include "../include/standard_allocator.h"
#include "../include/exceptions.h"

#include "../include/node_program.h"

piranha::IrNode::IrNode() {
	m_attributes = nullptr;
	m_isInterface = false;
	m_definition = nullptr;
}

piranha::IrNode::IrNode(const IrTokenInfo_string &type, const IrTokenInfo_string &name, 
			IrAttributeList *attributes, const IrTokenInfo_string &library) {
	m_type = type;
	m_name = name;
	m_attributes = attributes;
	m_library = library;

	registerToken(&type);
	registerToken(&name);
	registerToken(&library);
	registerComponent(attributes);

	m_definition = nullptr;
	m_isInterface = false;
}

piranha::IrNode::IrNode(const IrTokenInfo_string &type, IrAttributeList *attributes,
			const IrTokenInfo_string &library) {
	m_type = type;
	m_name = IrTokenInfo_string("");
	m_attributes = attributes;
	m_library = library;

	registerToken(&type);
	registerToken(&library);
	registerComponent(attributes);

	m_definition = nullptr;
	m_isInterface = false;
}

piranha::IrNode::~IrNode() {
	/* void */
}

const piranha::ChannelType *piranha::IrNode::getImmediateChannelType() {
	return (m_definition != nullptr)
		? m_definition->getChannelType()
		: nullptr;
}

void piranha::IrNode::setAttributes(IrAttributeList *list) {
	m_attributes = list;
	registerComponent(list);
}

piranha::IrAttribute *piranha::IrNode::getAttribute(const std::string &name, int *count) const {
	if (m_attributes == nullptr) return nullptr;

	if (count != nullptr) *count = 0;

	IrAttribute *result = nullptr;
	int attributeCount = m_attributes->getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		IrAttribute *attribute = m_attributes->getAttribute(i);
		bool definitionMatches = attribute->getAttributeDefinition() != nullptr
			? attribute->getAttributeDefinition()->getName() == name
			: false;
		if (definitionMatches || attribute->getName() == name) {
			if (result == nullptr) result = attribute;
			if (count != nullptr) (*count)++;
		}
	}

	return result;
}

void piranha::IrNode::setScopeParent(IrParserStructure *parentScope) {
	IrParserStructure::setScopeParent(parentScope);
	if (m_attributes != nullptr) {
		m_attributes->setScopeParent(parentScope);
	}
}

piranha::IrAttribute *piranha::IrNode::getAttribute(IrAttributeDefinition *definition, int *count) const {
	if (m_attributes == nullptr) return nullptr;

	if (count != nullptr) *count = 0;

	IrAttribute *result = nullptr;
	int attributeCount = m_attributes->getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		IrAttribute *attribute = m_attributes->getAttribute(i);
		if (attribute->getAttributeDefinition() == definition) {
			if (result == nullptr) result = attribute;
			if (count != nullptr) (*count)++;
		}
	}

	return result;
}

piranha::IrParserStructure *piranha::IrNode::resolveLocalName(const std::string &name) const {
	IrParserStructure *attribute = getAttribute(name);
	if (attribute != nullptr) return attribute;
	
	if (m_definition != nullptr) return m_definition->resolveLocalName(name);
	else return nullptr;
}

void piranha::IrNode::_resolveDefinitions() {
	resolveNodeDefinition();
	resolveAttributeDefinitions();
}

void piranha::IrNode::_validate() {
	IrAttributeList *attributes = m_attributes;
	IrCompilationUnit *unit = getParentUnit();

	if (attributes == nullptr) {
		// There was a syntax error before this step
		return;
	}

	// Check for symbols used more than once
	int attributeCount = attributes->getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		IrAttribute *attribute = attributes->getAttribute(i);

		int count;
		bool positional = attribute->isPositional();

		IrAttributeDefinition *definition = attribute->getAttributeDefinition();

		// If there was an error resolving the definition, skip this validation step
		if (definition != nullptr) getAttribute(definition, &count);
		else continue;

		if (count > 1) {
			// Attribute defined multiple times

			if (positional) {
				// Log a more specific message for clarify if the attribute is positional
				unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(),
					ErrorCode::InputSpecifiedMultipleTimesPositional));
			}
			else {
				unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(),
					ErrorCode::InputSpecifiedMultipleTimes));
			}
		}
	}

	// Check for missing inputs
	if (m_definition != nullptr) {
		auto attributeList = m_definition->getAttributeDefinitionList();

		// Check that there wasn't an error in the definition
		if (attributeList != nullptr) {
			int inputCount = attributeList->getInputCount();

			for (int i = 0; i < inputCount; i++) {
				IrAttributeDefinition *input = attributeList->getInputDefinition(i);
				IrParserStructure *attribute = getAttribute(input);

				if (attribute == nullptr && input->getDefaultValue() == nullptr) {
					// This input port is not conencted and has no default value
					unit->addCompilationError(new CompilationError(*getSummaryToken(),
						ErrorCode::InputNotConnected));
				}
			}
		}
	}
}

void piranha::IrNode::_checkInstantiation() {
	// Check all references relating to the connection of inputs from this
	// node to the actual definition.
	IrContextTree *parentContext = new IrContextTree(nullptr);
	IrContextTree *mainContext = parentContext->newChild(this, true);

	if (m_definition != nullptr) {
		m_definition->checkReferences(mainContext);
	}
}

void piranha::IrNode::_expand() {
	IrContextTree *parentContext = new IrContextTree(nullptr);
	IrContextTree *mainContext = parentContext->newChild(this, true);
	if (m_definition != nullptr) {
		m_definition->expand(mainContext);
	}
}

void piranha::IrNode::_expand(IrContextTree *context) {
	IrContextTree *mainContext = context->newChild(this, false);
	if (m_definition != nullptr) {
		m_definition->expand(mainContext);
	}
}

void piranha::IrNode::_checkTypes() {
	IrContextTree *parentContext = new IrContextTree(nullptr);
	IrContextTree *mainContext = parentContext->newChild(this, true);
	if (m_definition != nullptr) {
		m_definition->checkTypes(mainContext);
	}
}

void piranha::IrNode::_checkTypes(IrContextTree *context) {
	IrContextTree *mainContext = context->newChild(this, false);
	if (m_definition != nullptr) {
		m_definition->checkTypes(mainContext);
	}
}

void piranha::IrNode::resolveNodeDefinition() {
	// In case the definition for this node has been specified beforehand
	if (m_definition != nullptr) return;

	int definitionCount = 0;
	IrNodeDefinition *definition = nullptr;
	IrCompilationUnit *unit = getParentUnit();

	if (m_library.specified) {
		if (!m_library.data.empty()) {
			definition = unit->resolveNodeDefinition(getType(), &definitionCount, m_library.data);
		}
		else {
			// Adding an empty library name means that the local scope must strictly be used
			definition = unit->resolveLocalNodeDefinition(getType(), &definitionCount);
		}
	}
	else definition = unit->resolveNodeDefinition(getType(), &definitionCount, "");

	if (definitionCount > 0) {
		// TODO: log a warning when a node type is ambiguous
	}

	if (definition == nullptr) {
		unit->addCompilationError(new CompilationError(getTypeToken(), 
			ErrorCode::UndefinedNodeType));
	}
	else setDefinition(definition);
}

void piranha::IrNode::resolveAttributeDefinitions() {
	if (m_definition == nullptr) {
		// The definition wasn't found so resolving any attributes doesn't make sense
		return;
	}

	if (m_attributes == nullptr) {
		// There was a compilation error in the attributes section, so this step can be skipped
		return;
	}

	IrCompilationUnit *unit = getParentUnit();

	int attributeCount = m_attributes->getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		IrAttribute *attribute = m_attributes->getAttribute(i);

		IrAttributeDefinition *definition;
		
		if (attribute->isPositional()) {
			const IrAttributeDefinitionList *list = m_definition->getAttributeDefinitionList();
			int position = attribute->getPosition();

			// Check position is not out of bounds
			if (position >= list->getInputCount()) {
				unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
					ErrorCode::ArgumentPositionOutOfBounds));
				attribute->setAttributeDefinition(nullptr);
				return;
			}
			else {
				definition = m_definition->getAttributeDefinitionList()->getInputDefinition(
					attribute->getPosition()
				);
			}
		}
		else definition = m_definition->getAttributeDefinition(attribute->getName());

		if (definition == nullptr) {
			// Port not found
			unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
				ErrorCode::PortNotFound));
			attribute->setAttributeDefinition(nullptr);
		}
		else if (definition->getDirection() == IrAttributeDefinition::OUTPUT) {
			// Can't assign an output port
			unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
				ErrorCode::UsingOutputPortAsInput));
			attribute->setAttributeDefinition(nullptr);
		}
		else {
			// Port is good
			attribute->setAttributeDefinition(definition);
		}
	}
}

piranha::Node *piranha::IrNode::_generateNode(IrContextTree *context, NodeProgram *program) {
	if (isInterface()) {
		NodeOutput *parentNodeOutput = getScopeParent()->generateNodeOutput(context, program);

		if (parentNodeOutput == nullptr) {
			int a = 0;
		}

		return parentNodeOutput->getInterface();
	}

	IrContextTree *newContext;
	IrContextTree *parentContext = context;
	if (parentContext == nullptr) parentContext = new IrContextTree(nullptr);

	newContext = parentContext->newChild(this);

	IrNodeDefinition *definition = getDefinition();
	const IrAttributeDefinitionList *allAttributes = definition->getAttributeDefinitionList();
	const IrAttributeList *specifiedAttributes = getAttributes();
	int attributeCount = allAttributes->getDefinitionCount();

	struct Mapping {
		std::string name;
		NodeOutput *output;
		bool primary;
	};

	std::vector<Mapping> inputs;
	std::vector<Mapping> outputs;

	for (int i = 0; i < attributeCount; i++) {
		IrAttributeDefinition *attributeDefinition = allAttributes->getDefinition(i);

		if (attributeDefinition->getDirection() == IrAttributeDefinition::INPUT) {
			IrAttribute *attribute = specifiedAttributes->getAttribute(attributeDefinition);

			if (attribute != nullptr) {
				// Input was specified
				IrReferenceInfo info;
				IrReferenceQuery query;
				query.inputContext = parentContext;
				query.recordErrors = false;
				IrParserStructure *asValue = attribute->getReference(query, &info);

				Mapping inputPort;
				inputPort.output = asValue->generateNodeOutput(info.newContext, program);
				inputPort.name = attributeDefinition->getName();
				inputs.push_back(inputPort);
			}
			else {
				// Use the default value in the definition
				IrReferenceInfo info;
				IrReferenceQuery query;
				query.inputContext = parentContext;
				query.recordErrors = false;
				IrParserStructure *asValue = attributeDefinition->getReference(query, &info);

				Mapping inputPort;
				inputPort.output = asValue->generateNodeOutput(info.newContext, program);
				inputPort.name = attributeDefinition->getName();
				inputs.push_back(inputPort);
			}
		}
		else if (attributeDefinition->getDirection() == IrAttributeDefinition::OUTPUT &&
																	!definition->isBuiltin()) {
			IrReferenceInfo info;
			IrReferenceQuery query;
			query.inputContext = newContext;
			query.recordErrors = false;
			IrParserStructure *reference = attributeDefinition->getReference(query, &info);

			Mapping outputPort;
			outputPort.output = reference->generateNodeOutput(info.newContext, program);
			outputPort.name = attributeDefinition->getName();
			outputPort.primary = (reference == getDefaultOutputValue());

			outputs.push_back(outputPort);
		}
	}

	// Generate internal nodes
	IrNodeList *nestedNodeList = definition->getBody();
	if (nestedNodeList != nullptr) {
		int nestedNodeCount = nestedNodeList->getItemCount();
		for (int i = 0; i < nestedNodeCount; i++) {
			IrNode *node = nestedNodeList->getItem(i);
			node->generateNode(newContext, program);
		}
	}

	int inputCount = (int)inputs.size();
	int outputCount = (int)outputs.size();

	Node *newNode = program->getRules()->generateNode(this, parentContext);
	if (newNode != nullptr) {
		newNode->setName(getName());
		newNode->setIrStructure(this);
		newNode->initialize();

		for (int i = 0; i < inputCount; i++) {
			pNodeInput input = inputs[i].output;
			std::string name = inputs[i].name;

			const ChannelType *conversionType = newNode->getConversion(input, name);
			
			if (conversionType != nullptr) {
				// A conversion is needed
				const ChannelType *originalType = input->getType();
				Node *conversionNode = program->getRules()->generateConversion({ originalType, conversionType });

				if (conversionNode == nullptr) throw MissingConversion();

				conversionNode->setName("$autoconversion");
				conversionNode->setIrStructure(nullptr);
				conversionNode->setIrContext(nullptr);
				conversionNode->initialize();

				if (conversionNode == nullptr) {
					// TODO: Error for no valid conversion being found
				}
				else {
					conversionNode->connectDefaultInput(input);
					input = conversionNode->getPrimaryOutput();
				}
			}

			newNode->connectInput(input, name.c_str());
		}

		// Add the new node to the program
		program->addNode(newNode);
	}

	return newNode;
}

piranha::NodeOutput *piranha::IrNode::_generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	if (isInterface()) return getScopeParent()->generateNodeOutput(context, program);
	else return nullptr;
}

piranha::IrParserStructure *piranha::IrNode::getDefaultPort(bool *failed) {
	auto definition = m_definition;
	if (definition == nullptr) {
		*failed = true;
		return nullptr;
	}

	auto definitionList = definition->getAttributeDefinitionList();
	if (definitionList == nullptr) {
		*failed = true;
		return nullptr;
	}

	*failed = false;
	return definitionList->getAliasOutput();
}

piranha::IrValue *piranha::IrNode::getDefaultOutputValue() {
	auto definition = m_definition;
	if (definition == nullptr) return nullptr;

	auto definitionList = definition->getAttributeDefinitionList();
	if (definitionList == nullptr) return nullptr;

	auto outputDefinition = definitionList->getAliasOutput();
	if (outputDefinition == nullptr) return nullptr;

	auto defaultValue = outputDefinition->getDefaultValue();
	if (defaultValue == nullptr) return nullptr;

	return defaultValue->getAsValue();
}

void piranha::IrNode::writeTraceToFile(std::ofstream &file) {
	IrContextTree *parentContext = new IrContextTree(nullptr);
	IrContextTree *thisContext = parentContext->newChild(this);

	int attributeDefinitions = m_definition->getAttributeDefinitionList()->getDefinitionCount();
	for (int i = 0; i < attributeDefinitions; i++) {
		m_definition
			->getAttributeDefinitionList()
			->getDefinition(i)
			->writeReferencesToFile(file, thisContext);
	}
}

piranha::IrParserStructure *piranha::IrNode::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	IR_RESET(query);

	bool failed = false;
	IrParserStructure *aliasPort = getDefaultPort(&failed);

	if (failed) {
		IR_FAIL();
		return nullptr;
	}

	if (aliasPort != nullptr) {
		IrContextTree *newContext = query.inputContext->newChild(this, false);
		IR_INFO_OUT(newContext, newContext);

		return aliasPort;
	}

	return nullptr;
}

void piranha::IrNode::checkReferences(IrContextTree *inputContext) {
	IrContextTree *thisContext = inputContext->newChild(this, false);
	IrAttributeList *attributes = getAttributes();

	if (attributes != nullptr) {
		int attributeCount = attributes->getAttributeCount();
		for (int i = 0; i < attributeCount; i++) {
			attributes->getAttribute(i)->checkReferences(inputContext);
		}
	}

	if (m_definition != nullptr) {
		m_definition->checkReferences(thisContext);
	}
}
