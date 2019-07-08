#include <constructed_string_node_output.h>

piranha::ConstructedStringNodeOutput::ConstructedStringNodeOutput() {
	/* void */
}

piranha::ConstructedStringNodeOutput::~ConstructedStringNodeOutput() {
	/* void */
}

void piranha::ConstructedStringNodeOutput::fullCompute(void *_target) const {
	std::string value;
	m_stringInput->fullCompute((void *)&value);
	
	std::string *target = reinterpret_cast<std::string *>(_target);
	*target = value;
}

void piranha::ConstructedStringNodeOutput::registerInputs() {
	registerInput(&m_stringInput);
}
