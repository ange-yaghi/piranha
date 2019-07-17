#include "../include/ir_attribute_definition.h"

#include "../include/ir_value.h"
#include "../include/ir_input_connection.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"
#include "../include/ir_context_tree.h"
#include "../include/language_rules.h"
#include "../include/ir_node_definition.h"

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

void piranha::IrAttributeDefinition::setDefaultToken(const IrTokenInfo_string &defaultToken) {
	m_defaultToken = defaultToken;
	registerToken(&m_defaultToken);
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
			IR_DEAD_END();
			return nullptr;
		}
		else {
			// This is the result of an unconnected input (that has no input)
			IR_FAIL();
			return nullptr;
		}
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
