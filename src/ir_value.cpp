#include "../include/ir_value.h"

piranha::IrValue::IrValue(piranha::IrValue::ValueType type) {
    m_type = type;
}

piranha::IrValue::~IrValue() {
    /* void */
}
