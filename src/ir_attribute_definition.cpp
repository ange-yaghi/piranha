#include "../include/ir_attribute_definition.h"

#include "../include/ir_value.h"
#include "../include/ir_input_connection.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"
#include "../include/ir_context_tree.h"

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
			IR_INFO_OUT(touchedMainContext, query.inputContext->isMainContext());

			return reference;
		}
	}

	// An attribute definition will by default point to its definition (ie default value)
	if (m_defaultValue == nullptr) {
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
