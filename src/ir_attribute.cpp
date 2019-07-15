#include "../include/ir_attribute.h"

#include "../include/ir_value.h"

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
