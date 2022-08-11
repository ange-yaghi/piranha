#include "float_conversions.h"

#include <sstream>

// CONVERSION =============================================
// string -> float
piranha::StringToFloatConversionOutput::StringToFloatConversionOutput() {
    /* void */
}

piranha::StringToFloatConversionOutput::~StringToFloatConversionOutput() {
    /* void */
}

void piranha::StringToFloatConversionOutput::fullCompute(void *_target) const {
    std::string value;
    m_input->fullCompute((void *)&value);

    piranha::native_float floatValue;
    std::stringstream ss(value);
    ss >> floatValue;

    piranha::native_float *target = reinterpret_cast<piranha::native_float *>(_target);
    *target = floatValue;
}

void piranha::StringToFloatConversionOutput::registerInputs() {
    registerInput(&m_input);
}

// CONVERSION =============================================
// int -> float
piranha::IntToFloatConversionOutput::IntToFloatConversionOutput() {
    /* void */
}

piranha::IntToFloatConversionOutput::~IntToFloatConversionOutput() {
    /* void */
}

void piranha::IntToFloatConversionOutput::fullCompute(void *_target) const {
    int value;
    m_input->fullCompute((void *)&value);

    piranha::native_float *target = reinterpret_cast<piranha::native_float *>(_target);
    *target = (piranha::native_float)value;
}

void piranha::IntToFloatConversionOutput::registerInputs() {
    registerInput(&m_input);
}
