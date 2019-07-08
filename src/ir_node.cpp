#include "ir_node.h"

#include "ir_attribute.h"
#include "ir_attribute_list.h"
#include "ir_compilation_unit.h"
#include "ir_node_definition.h"
#include "ir_attribute_definition.h"
#include "ir_compilation_error.h"
#include "ir_attribute_definition_list.h"
#include "ir_value.h"
#include <node.h>
#include <custom_node.h>
#include <standard_allocator.h>
#include "ir_context_tree.h"

#include <constructed_float_node.h>
#include <constructed_string_node.h>
#include <node_program.h>

piranha::IrNode::IrNode() {
	/* void */
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
}

piranha::IrNode::~IrNode() {
	/* void */
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

void piranha::IrNode::setParentScope(IrParserStructure *parentScope) {
	IrParserStructure::setParentScope(parentScope);
	if (m_attributes != nullptr) {
		m_attributes->setParentScope(parentScope);
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
				unit->addCompilationError(new IrCompilationError(*attribute->getSummaryToken(),
					IrErrorCode::InputSpecifiedMultipleTimesPositional));
			}
			else {
				unit->addCompilationError(new IrCompilationError(*attribute->getSummaryToken(),
					IrErrorCode::InputSpecifiedMultipleTimes));
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
					unit->addCompilationError(new IrCompilationError(*getSummaryToken(),
						IrErrorCode::InputNotConnected));
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

	IrAttributeList *attributes = getAttributes();
	if (attributes != nullptr) {
		int attributeCount = attributes->getAttributeCount();
		for (int i = 0; i < attributeCount; i++) {
			attributes->getAttribute(i)->checkReferences(parentContext);
		}
	}

	if (m_definition != nullptr) {
		m_definition->checkReferences(mainContext);
	}
}

void piranha::IrNode::resolveNodeDefinition() {
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
	else {
		definition = unit->resolveNodeDefinition(getType(), &definitionCount, "");
	}

	if (definitionCount > 0) {
		// TODO: log a warning when a node type is ambiguous
	}

	if (definition == nullptr) {
		unit->addCompilationError(new IrCompilationError(getTypeToken(), 
			IrErrorCode::UndefinedNodeType));
	}

	else {
		setDefinition(definition);
	}
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
				unit->addCompilationError(new IrCompilationError(*attribute->getSummaryToken(), 
					IrErrorCode::ArgumentPositionOutOfBounds));
				attribute->setAttributeDefinition(nullptr);
				return;
			}
			else {
				definition = m_definition->getAttributeDefinitionList()->getInputDefinition(
					attribute->getPosition()
				);
			}
		}
		else {
			definition = m_definition->getAttributeDefinition(attribute->getName());
		}

		if (definition == nullptr) {
			// Port not found
			unit->addCompilationError(new IrCompilationError(*attribute->getSummaryToken(), 
				IrErrorCode::PortNotFound));
			attribute->setAttributeDefinition(nullptr);
		}
		else if (definition->getDirection() == IrAttributeDefinition::OUTPUT) {
			// Can't assign an output port
			unit->addCompilationError(new IrCompilationError(*attribute->getSummaryToken(), 
				IrErrorCode::UsingOutputPortAsInput));
			attribute->setAttributeDefinition(nullptr);
		}
		else {
			// Port is good
			attribute->setAttributeDefinition(definition);
		}
	}
}

piranha::Node *piranha::IrNode::generateNode(IrContextTree *context, NodeProgram *program) {
	IrContextTree *newContext;
	IrContextTree *parentContext = context;
	if (parentContext == nullptr) {
		parentContext = new IrContextTree(nullptr);
	}

	newContext = parentContext->newChild(this);

	NodeTableEntry *entry = getTableEntry(parentContext);
	if (entry != nullptr) return entry->generatedNode;

	entry = newTableEntry(parentContext);

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
				IrReferenceQuery query;
				query.inputContext = parentContext;
				query.recordErrors = false;
				IrValue *asValue = attribute->getImmediateReference(query, nullptr)->getAsValue();

				Mapping inputPort;
				inputPort.output = asValue->generateNodeOutput(parentContext, program);
				inputPort.name = attributeDefinition->getName();
				inputs.push_back(inputPort);
			}
			else {
				// Use the default value in the definition
				IrReferenceQuery query;
				query.inputContext = parentContext;
				query.recordErrors = false;
				IrValue *asValue = attributeDefinition->getImmediateReference(query, nullptr)->getAsValue();

				Mapping inputPort;
				inputPort.output = asValue->generateNodeOutput(parentContext, program);
				inputPort.name = attributeDefinition->getName();
				inputs.push_back(inputPort);
			}
		}
		else if (attributeDefinition->getDirection() == IrAttributeDefinition::OUTPUT &&
																	!definition->isBuiltin()) {
			IrReferenceQuery query;
			query.inputContext = parentContext;
			query.recordErrors = false;
			IrValue *asValue = attributeDefinition->getImmediateReference(query, nullptr)->getAsValue();

			Mapping outputPort;
			outputPort.output = asValue->generateNodeOutput(newContext, program);
			outputPort.name = attributeDefinition->getName();
			outputPort.primary = (asValue == getDefaultOutputValue());

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

	Node *newNode = nullptr;
	if (definition->isBuiltin()) {
		if (definition->getBuiltinName() == "__piranha__string") {
			newNode = StandardAllocator::Global()->allocate<ConstructedStringNode>();
		}
		else if (definition->getBuiltinName() == "__piranha__float") {
			newNode = StandardAllocator::Global()->allocate<ConstructedFloatNode>();
		}
	}
	else {
		CustomNode *newCustomNode = StandardAllocator::Global()->allocate<CustomNode>();

		for (int i = 0; i < inputCount; i++) {
			newCustomNode->addCustomInput(inputs[i].name);
		}

		for (int i = 0; i < outputCount; i++) {
			newCustomNode->addCustomOutput(outputs[i].output, outputs[i].name, outputs[i].primary);
		}

		newNode = newCustomNode;
	}

	if (newNode != nullptr) {
		newNode->setName(getName());
		newNode->setIrNode(this);
		newNode->initialize();

		for (int i = 0; i < inputCount; i++) {
			newNode->connectInput(inputs[i].output, inputs[i].name.c_str());
		}
	}

	// Add the new node to the program
	program->addNode(newNode);

	entry->generatedNode = newNode;
	return newNode;
}

piranha::IrNode::NodeTableEntry *piranha::IrNode::getTableEntry(IrContextTree *context) {
	int entryCount = (int)m_nodeTable.size();
	for (int i = 0; i < entryCount; i++) {
		if (m_nodeTable[i]->context->isEqual(context)) {
			return m_nodeTable[i];
		}
	}

	return nullptr;
}

piranha::IrNode::NodeTableEntry *piranha::IrNode::newTableEntry(IrContextTree *context) {
	NodeTableEntry *newEntry = new NodeTableEntry();
	newEntry->context = context;
	newEntry->generatedNode = nullptr;

	m_nodeTable.push_back(newEntry);

	return newEntry;
}

piranha::IrValue *piranha::IrNode::getDefaultOutputValue() {
	auto definition = m_definition;
	if (definition == nullptr) return nullptr;

	auto definitionList = definition->getAttributeDefinitionList();
	if (definitionList == nullptr) return nullptr;

	auto outputDefinition = definitionList->getDefaultOutput();
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