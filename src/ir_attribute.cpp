#include "../include/ir_attribute.h"

#include "../include/ir_value.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_node.h"
#include "../include/language_rules.h"
#include "../include/compilation_error.h"
#include "../include/ir_compilation_unit.h"

piranha::IrAttribute::IrAttribute() {
	/* void */
}

piranha::IrAttribute::IrAttribute(const IrTokenInfo_string &name, IrValue *value) {
	m_name = name;
	m_value = value;
	m_position = -1;

	registerToken(&name);
	registerComponent(value);

	m_definition = nullptr;
}

piranha::IrAttribute::IrAttribute(IrValue *value) {
	m_value = value;
	m_position = -1;

	registerComponent(value);
}

piranha::IrAttribute::~IrAttribute() {
	/* void */
}

void piranha::IrAttribute::setValue(IrValue *value) {
	m_value = value;
	registerComponent(value);
}

piranha::IrParserStructure *piranha::IrAttribute::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	IR_INFO_OUT(err, nullptr);
	IR_INFO_OUT(newContext, query.inputContext);
	IR_INFO_OUT(failed, false);

	return m_value;
}

void piranha::IrAttribute::_checkType(IrParserStructure *finalReference, IrContextTree *context) {
	if (m_definition == nullptr) return;

	IrNodeDefinition *typeDefinition = m_definition->getTypeDefinition();

	if (typeDefinition != nullptr && m_definition->getDirection() == IrAttributeDefinition::INPUT) {
		IrNode *refAsNode = finalReference->getAsNode();

		bool incorrectNodeType = false;
		if (refAsNode != nullptr) {
			IrNodeDefinition *definition = refAsNode->getDefinition();
			if (definition == nullptr) return;
			if (definition == typeDefinition) return; // Type is confirmed to be correct

			incorrectNodeType = true;
		}

		if (m_rules == nullptr) return;

		const ChannelType *type = finalReference->getImmediateChannelType();
		const ChannelType *expectedType = m_rules->resolveChannelType(typeDefinition->getBuiltinName());

		if (type == expectedType) {
			if (expectedType != nullptr) return; // No conversion necessary
		}

		bool validConversion = m_rules->checkConversion({ type, expectedType });
		if (validConversion) return;

		IrCompilationUnit *unit = getParentUnit();
		unit->addCompilationError(new CompilationError(*getSummaryToken(),
			ErrorCode::IncompatibleType, context));
	}
}
