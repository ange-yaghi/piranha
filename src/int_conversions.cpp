#include "int_conversions.h"

// CONVERSION =============================================
// string -> int
piranha::StringToIntConversion::StringToIntConversion() {
    /* void */
}

piranha::StringToIntConversion::~StringToIntConversion() {
    /* void */
}

void piranha::StringToIntConversion::fullCompute(void *_target) const {
    std::string value;
    m_input->fullCompute((void *)&value);

    piranha::native_int intValue;
    std::stringstream ss(value);
    ss >> intValue;

    piranha::native_int *target = reinterpret_cast<native_int *>(_target);
    *target = intValue;
}

void piranha::StringToIntConversion::registerInputs() {
    registerInput(&m_input);
}
