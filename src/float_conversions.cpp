#include "..\include\float_conversions.h"

#include <sstream>

piranha::StringToFloatConversionOutput::StringToFloatConversionOutput() {
	/* void */
}

piranha::StringToFloatConversionOutput::~StringToFloatConversionOutput() {
	/* void */
}

void piranha::StringToFloatConversionOutput::fullCompute(void *_target) const {
	std::string value;
	m_input->fullCompute((void *)&value);

	double floatValue;
	std::stringstream ss(value);
	ss >> floatValue;

	double *target = reinterpret_cast<double *>(_target);
	*target = floatValue;
}

void piranha::StringToFloatConversionOutput::registerInputs() {
	registerInput(&m_input);
}
