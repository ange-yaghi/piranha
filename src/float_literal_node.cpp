#include "..\include\float_literal_node.h"

piranha::FloatLiteralNode::FloatLiteralNode() {
	/* void */
}

piranha::FloatLiteralNode::~FloatLiteralNode() {
	/* void */
}

void piranha::FloatLiteralNode::_initialize() {
	/* void */
}

void piranha::FloatLiteralNode::_evaluate() {
	m_output.setValue(m_value);
}

void piranha::FloatLiteralNode::_destroy() {
	/* void */
}

void piranha::FloatLiteralNode::registerOutputs() {
	setPrimaryOutput(&m_output);

	registerOutput(&m_output, "$primary");
}
