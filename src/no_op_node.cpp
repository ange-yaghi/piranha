#include "../include/no_op_node.h"

piranha::NoOpNode::NoOpNode() {
    /* void */
}

piranha::NoOpNode::~NoOpNode() {
    /* void */
}

void piranha::NoOpNode::_initialize() {
    /* void */
}

void piranha::NoOpNode::_evaluate() {
    /* void */
}

void piranha::NoOpNode::_destroy() {
    /* void */
}

void piranha::NoOpNode::registerOutputs() {
    setPrimaryOutput("__out");
    registerOutputReference(&m_input, "__out");
}

void piranha::NoOpNode::registerInputs() {
    registerInput(&m_input, "__in");
}
