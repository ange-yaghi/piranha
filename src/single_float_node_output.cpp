#include <single_float_node_output.h>

piranha::SingleFloatNodeOutput::SingleFloatNodeOutput() {
	m_value = (double)0.0;
}

piranha::SingleFloatNodeOutput::~SingleFloatNodeOutput() {
	/* void */
}


void piranha::SingleFloatNodeOutput::fullOutput(void *_target) const {
	double *target = reinterpret_cast<double *>(_target);

	*target = m_value;
}
