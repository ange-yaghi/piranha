#include "../include/ir_attribute_definition.h"

#include "../include/ir_value.h"
#include "../include/ir_input_connection.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"
#include "../include/ir_context_tree.h"
#include "../include/language_rules.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_node.h"
#include "../include/node_output.h"
#include "../include/ir_attribute.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_value_constant.h"

piranha::IrAttributeDefinition::IrAttributeDefinition(const IrTokenInfo_string &directionToken,
								const IrTokenInfo_string &name, DIRECTION dir) {
	m_name = name;
	registerToken(&m_name);

	m_directionToken = directionToken;
	registerToken(&m_directionToken);

	m_direction = dir;

	if (m_direction == OUTPUT) {
		setVisibility(IrVisibility::PUBLIC);
	}
}

piranha::IrAttributeDefinition::IrAttributeDefinition(const IrTokenInfo_string &name) {
	m_name = name;
	registerToken(&m_name);

	m_direction = OUTPUT;
	setVisibility(IrVisibility::PUBLIC);
}

piranha::IrAttributeDefinition::~IrAttributeDefinition() {
	/* void */
}

void piranha::IrAttributeDefinition::setDefaultValue(IrValue *value) {
	m_defaultValue = value;
	registerComponent(m_defaultValue);
}

void piranha::IrAttributeDefinition::setAliasToken(const IrTokenInfo_string &aliasToken) {
	m_aliasToken = aliasToken;
	registerToken(&m_aliasToken);
}

piranha::IrInputConnection *piranha::IrAttributeDefinition::getImpliedMember(const std::string &name) const {
	int count = getImpliedMemberCount();

	for (int i = 0; i < count; i++) {
		if (m_impliedMembers[i]->getMember() == name) {
			return m_impliedMembers[i];
		}
	}

	return nullptr;
}

piranha::IrParserStructure *piranha::IrAttributeDefinition::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	IR_RESET(query);

	// First check the input context for the reference
	if (!IR_EMPTY_CONTEXT()) {
		IrParserStructure *reference = query.inputContext->resolveDefinition(this);
		if (reference != nullptr) {
			IrNode **expansion = m_expansions.lookup(query.inputContext);
			if (*expansion != nullptr) return *expansion;

			IR_INFO_OUT(newContext, query.inputContext->getParent());

			// This flag must be set to notify that this chain of resolutions actually
			// touched the important main context (the only context for which errors
			// are being logged)
			IR_INFO_OUT(touchedMainContext, query.inputContext->isMainContext());

			return reference;
		}
	}

	// An attribute definition will by default point to its definition (ie default value)
	if (m_defaultValue == nullptr && m_direction == INPUT) {
		if (IR_EMPTY_CONTEXT()) {
			IrNode **expansion = m_expansions.lookup(query.inputContext);

			if (*expansion == nullptr) {
				IR_DEAD_END();
				return nullptr;
			}

			return *expansion;
		}
		else {
			// This is the result of an unconnected input (that has no default)
			IR_FAIL();
			return nullptr;
		}
	}
	else if (m_defaultValue == nullptr && m_direction == OUTPUT) {
		if (m_typeDefinition == nullptr) {
			IR_FAIL();
			return nullptr;
		}
		else {
			IrNode **node = m_expansions.lookup(query.inputContext);
			return *node;
		}
	}

	// Look up the expansion for the default case and return it if it exists
	IrNode **expansion = m_expansions.lookup(query.inputContext);
	if (expansion == nullptr) {
		IR_FAIL();
		return nullptr;
	}

	if (*expansion != nullptr) {
		return *expansion;
	}

	return m_defaultValue;
}

const piranha::ChannelType *piranha::IrAttributeDefinition::getImmediateChannelType() {
	if (m_typeDefinition != nullptr) {
		if (!m_typeDefinition->isBuiltin()) return nullptr;
		std::string builtinType = m_typeDefinition->getBuiltinName();

		const ChannelType *wholeType = m_rules->resolveChannelType(builtinType);
		return wholeType;
	}
	else return nullptr;
}

void piranha::IrAttributeDefinition::_expand(IrContextTree *context) {
	if (m_typeDefinition == nullptr) return;

	if (m_direction == OUTPUT && m_defaultValue == nullptr) {
		IrNode *expansion = new IrNode();
		expansion->setLogicalParent(this);
		expansion->setScopeParent(this);
		expansion->setInterface(true);
		expansion->setDefinition(m_typeDefinition);
		expansion->setRules(m_rules);
		expansion->expand(context);

		*m_expansions.newValue(context) = expansion;
	}
	else {
		IrReferenceInfo info;
		IrReferenceQuery query;
		query.inputContext = context;
		query.recordErrors = false;
		IrParserStructure *immediateReference = getImmediateReference(query, &info);

		if (info.failed) return;

		if (info.reachedDeadEnd) {
			IrNode *expansion = new IrNode();
			expansion->setLogicalParent(this);
			expansion->setScopeParent(this);
			expansion->setInterface(true);
			expansion->setDefinition(m_typeDefinition);
			expansion->setRules(m_rules);
			expansion->expand(context);

			*m_expansions.newValue(context) = expansion;

			return;
		}

		immediateReference->expandChain(context);
		IrParserStructure *reference = immediateReference->resolveToSingleChannel(query, &info);

		if (info.failed) return;
		if (info.reachedDeadEnd) return;

		IrNode *asNode = reference->getAsNode();

		bool nonMatchingTypes = false;
		if (asNode != nullptr && asNode->getDefinition() != m_typeDefinition) nonMatchingTypes = true;

		const ChannelType *referenceType = reference->getImmediateChannelType();
		const ChannelType *expectedType = m_rules->resolveChannelType(m_typeDefinition->getBuiltinName());

		if (referenceType == nullptr || expectedType == nullptr) {
			if (nonMatchingTypes) {
				// This error was already detected in an earlier step
				return;
			}
		}

		if (referenceType == expectedType) return; // No expansion/conversion needed

		std::string builtinType = m_rules->resolveConversionBuiltinType({ referenceType, expectedType });
		if (builtinType.empty()) return; // Incompatible types

		int count = 0;
		IrCompilationUnit *parentUnit = (context->getContext() == nullptr)
			? getParentUnit()
			: context->getContext()->getParentUnit();

		IrNodeDefinition *nodeDefinition = parentUnit->resolveBuiltinNodeDefinition(builtinType, &count);

		if (nodeDefinition == nullptr) {
			// No definition found for this builtin type
			int a = 0;
		}

		IrInternalReference *internalReference = new IrInternalReference(immediateReference);

		IrAttribute *input = new IrAttribute();
		input->setValue(internalReference);

		IrAttributeList *attributeList = new IrAttributeList();
		attributeList->addAttribute(input);

		IrNode *expansion = new IrNode();
		expansion->setAttributes(attributeList);
		expansion->setLogicalParent(this);
		expansion->setScopeParent(this);
		expansion->setDefinition(nodeDefinition);
		expansion->setRules(m_rules);
		expansion->expand(info.newContext);
		expansion->resolveDefinitions();

		*m_expansions.newValue(info.newContext) = expansion;
	}
}

void piranha::IrAttributeDefinition::_checkTypes(IrContextTree *context) {
	if (m_typeDefinition != nullptr && m_defaultValue != nullptr) {
		IrReferenceInfo info;
		IrReferenceQuery query;
		query.inputContext = context;
		query.recordErrors = false;
		IrParserStructure *defaultReference = m_defaultValue->getReference(query, &info);

		if (info.failed) return;
		if (info.reachedDeadEnd) return;

		if (!info.touchedMainContext && (context->getContext() != nullptr || !context->isMainContext())) return;

		bool incorrectNodeType = false;
		IrNode *refAsNode = defaultReference->getAsNode();
		if (refAsNode != nullptr) {
			IrNodeDefinition *definition = refAsNode->getDefinition();
			if (definition == nullptr) return;
			if (definition == m_typeDefinition) return; // Type is confirmed to be correct

			// Report an error
			incorrectNodeType = true;
		}

		if (m_rules == nullptr) return;

		const ChannelType *type = defaultReference->getImmediateChannelType();
		const ChannelType *expectedType = m_rules->resolveChannelType(m_typeDefinition->getBuiltinName());

		if (type == expectedType) {
			if (expectedType != nullptr) return; // No conversion necessary
		}

		bool validConversion = m_rules->checkConversion({ type, expectedType });
		if (validConversion) return;

		IrCompilationUnit *unit = getParentUnit();

		// Errors for inputs/outputs differ only in code but are fundamentally the same
		if (m_direction == INPUT) {
			unit->addCompilationError(new CompilationError(*m_defaultValue->getSummaryToken(),
				ErrorCode::IncompatibleDefaultType, context));
		}
		else if (m_direction == OUTPUT) {
			unit->addCompilationError(new CompilationError(*m_defaultValue->getSummaryToken(),
				ErrorCode::IncompatibleOutputDefinitionType, context));
		}
	}
}

void piranha::IrAttributeDefinition::_resolveDefinitions() {
	int definitionCount = 0;
	IrNodeDefinition *definition = nullptr;
	IrCompilationUnit *unit = getParentUnit();

	const IrTokenInfo_string &libraryToken = m_typeInfo.data[0];
	const IrTokenInfo_string &typeToken = m_typeInfo.data[1];

	// No action is needed if a type wasn't specified
	if (!typeToken.specified) return;

	const std::string &library = libraryToken.data;
	const std::string &type = typeToken.data;

	if (libraryToken.specified) {
		if (!library.empty()) {
			definition = unit->resolveNodeDefinition(type, &definitionCount, library);
		}
		else {
			// Adding an empty library name means that the local scope must strictly be used
			definition = unit->resolveLocalNodeDefinition(type, &definitionCount);
		}
	}
	else {
		if (typeToken.specified) {
			definition = unit->resolveNodeDefinition(type, &definitionCount, "");
		}
		else {
			// Type information was not given so skip the rest of the function
			m_typeDefinition = nullptr;
			return;
		}
	}

	if (definitionCount > 0) {
		// TODO: log a warning when a node type is ambiguous
	}

	if (definition == nullptr) {
		unit->addCompilationError(new CompilationError(typeToken,
			ErrorCode::UndefinedNodeType));
		m_typeDefinition = nullptr;
	}

	else m_typeDefinition = definition;
}

piranha::Node *piranha::IrAttributeDefinition::_generateNode(IrContextTree *context, NodeProgram *program) {
	if (m_typeDefinition != nullptr && m_direction == OUTPUT && m_defaultValue == nullptr) {
		// This must be an interface
		Node *parentNode = context->getContext()->generateNode(context->getParent(), program);
		NodeOutput *output = parentNode->getOutput(getName());

		return output->getInterface();
	}

	return nullptr;
}

piranha::NodeOutput *piranha::IrAttributeDefinition::_generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	if (m_typeDefinition != nullptr && m_direction == OUTPUT && m_defaultValue == nullptr) {
		// This must be an interface
		Node *parentNode = context->getContext()->generateNode(context->getParent(), program);
		NodeOutput *output = parentNode->getOutput(getName());

		return output;
	}

	return nullptr;
}
