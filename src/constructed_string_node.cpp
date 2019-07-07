#include <constructed_string_node.h>

piranha::ConstructedStringNode::ConstructedStringNode() {
	/* void */
}

piranha::ConstructedStringNode::~ConstructedStringNode() {
	/* void */
}

void piranha::ConstructedStringNode::_initialize() {
	m_output.initialize();
}

void piranha::ConstructedStringNode::_evaluate() {
	/* void */
}

void piranha::ConstructedStringNode::_destroy() {
	/* void */
}

void piranha::ConstructedStringNode::registerOutputs() {
	setPrimaryOutput(&m_output);
}

void piranha::ConstructedStringNode::registerInputs() {
	registerInput(m_output.getStringConnection(), "__in");
}
