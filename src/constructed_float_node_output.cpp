#include <constructed_float_node_output.h>

#include <float_node_output.h>
#include <string_node_output.h>
#include <literal_node_output.h>

#include <sstream>

piranha::ConstructedFloatNodeOutput::ConstructedFloatNodeOutput() {
	m_input = nullptr;
}

piranha::ConstructedFloatNodeOutput::~ConstructedFloatNodeOutput() {
	/* void */
}

void piranha::ConstructedFloatNodeOutput::fullCompute(void *_target) const {
	if (m_input->isType(FloatNodeOutput::FloatType)) {
		m_input->fullCompute(_target);
	}
	else if (m_input->isType(StringNodeOutput::StringType)) {
		std::string str;
		m_input->fullCompute(_target);

		std::stringstream ss(str);
		double v;
		ss >> v;

		double *target = reinterpret_cast<double *>(_target);
		*target = v;
	}
	else if (m_input->isType(LiteralNodeOutputBase::FloatType)) {
		m_input->fullCompute(_target);
	}
}

void piranha::ConstructedFloatNodeOutput::registerInputs() {
	registerInput(&m_input);
}
