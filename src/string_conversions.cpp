#include "../include/string_conversions.h"

#include <sstream>

// CONVERSION =============================================
// float -> string
piranha::FloatToStringConversionOutput::FloatToStringConversionOutput() {
    /* void */
}

piranha::FloatToStringConversionOutput::~FloatToStringConversionOutput() {
    /* void */
}

void piranha::FloatToStringConversionOutput::fullCompute(void *_target) const {
    piranha::native_float value;
    m_input->fullCompute((void *)&value);

    std::stringstream ss;
    ss << value;

    std::string *target = reinterpret_cast<std::string *>(_target);
    *target = ss.str();
}

void piranha::FloatToStringConversionOutput::registerInputs() {
    registerInput(&m_input);
}

// CONVERSION =============================================
// int -> string
piranha::IntToStringConversionOutput::IntToStringConversionOutput() {
    /* void */
}

piranha::IntToStringConversionOutput::~IntToStringConversionOutput() {
    /* void */
}

void piranha::IntToStringConversionOutput::fullCompute(void *_target) const {
    piranha::native_int value;
    m_input->fullCompute((void *)&value);

    std::stringstream ss;
    ss << value;

    std::string *target = reinterpret_cast<std::string *>(_target);
    *target = ss.str();
}

void piranha::IntToStringConversionOutput::registerInputs() {
    registerInput(&m_input);
}
