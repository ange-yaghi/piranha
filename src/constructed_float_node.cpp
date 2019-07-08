#include <constructed_float_node.h>

piranha::ConstructedFloatNode::ConstructedFloatNode() {
	/* void */
}

piranha::ConstructedFloatNode::~ConstructedFloatNode() {
	/* void */
}

void piranha::ConstructedFloatNode::_initialize() {
	/* void */
}

void piranha::ConstructedFloatNode::_evaluate() {
	/* void */
}

void piranha::ConstructedFloatNode::_destroy() {
	/* void */
}

void piranha::ConstructedFloatNode::registerOutputs() {
	setPrimaryOutput(&m_output);
	registerOutput(&m_output, "$primary");
}

void piranha::ConstructedFloatNode::registerInputs() {
	registerInput(m_output.getInputConnection(), "__in");
}
