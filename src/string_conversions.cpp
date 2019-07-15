#include "../include/string_conversions.h"

#include <sstream>

piranha::FloatToStringConversionOutput::FloatToStringConversionOutput() {
	/* void */
}

piranha::FloatToStringConversionOutput::~FloatToStringConversionOutput() {
	/* void */
}

void piranha::FloatToStringConversionOutput::fullCompute(void *_target) const {
	double value;
	m_input->fullCompute((void *)&value);

	std::stringstream ss;
	ss << value;

	std::string *target = reinterpret_cast<std::string *>(_target);
	*target = ss.str();
}

void piranha::FloatToStringConversionOutput::registerInputs() {
	registerInput(&m_input);
}
