#include "../include/ir_value.h"

#include "../include/node_program.h"
#include "../include/language_rules.h"

#include "../include/node.h"

piranha::IrValue::IrValue(piranha::IrValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::IrValue::~IrValue() {
	/* void */
}

bool piranha::IrValue::isGeneric() const {
	return (m_type == GENERIC);
}
