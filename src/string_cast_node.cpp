#include "../include/string_cast_node.h"

#include "../include/fundamental_types.h"

piranha::StringCastNode::StringCastNode() {
	/* void */
}

piranha::StringCastNode::~StringCastNode() {
	/* void */
}

void piranha::StringCastNode::_initialize() {
	/* void */
}

void piranha::StringCastNode::_evaluate() {
	/* void */
}

void piranha::StringCastNode::_destroy() {
	/* void */
}

void piranha::StringCastNode::registerOutputs() {
	setPrimaryOutputReference(&m_input);
	registerOutputReference(&m_input, "$primary");
}

void piranha::StringCastNode::registerInputs() {
	registerInput(&m_input, "__in", &FundamentalType::StringType);
}
