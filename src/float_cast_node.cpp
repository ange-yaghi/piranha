#include "../include/float_cast_node.h"

#include "../include/fundamental_types.h"

piranha::FloatCastNode::FloatCastNode() {
	/* void */
}

piranha::FloatCastNode::~FloatCastNode() {
	/* void */
}

void piranha::FloatCastNode::_initialize() {
	/* void */
}

void piranha::FloatCastNode::_evaluate() {
	/* void */
}

void piranha::FloatCastNode::_destroy() {
	/* void */
}

void piranha::FloatCastNode::registerOutputs() {
	setPrimaryOutputReference(&m_input);
	registerOutputReference(&m_input, "$primary");
}

void piranha::FloatCastNode::registerInputs() {
	registerInput(&m_input, "__in", &FundamentalType::FloatType);
}
