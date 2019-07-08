#include <single_string_node_output.h>

piranha::SingleStringNodeOutput::SingleStringNodeOutput() {
	m_value = "";
}

piranha::SingleStringNodeOutput::~SingleStringNodeOutput() {
	/* void */
}

void piranha::SingleStringNodeOutput::fullCompute(void *_target) const {
	std::string *target = static_cast<std::string *>(_target);
	*target = m_value;
}
