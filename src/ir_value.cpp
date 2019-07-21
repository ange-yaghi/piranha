#include "../include/ir_value.h"

piranha::IrValue::IrValue(piranha::IrValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::IrValue::~IrValue() {
	/* void */
}
