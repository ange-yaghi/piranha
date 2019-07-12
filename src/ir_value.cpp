#include "ir_value.h"

#include "node_program.h"
#include "language_rules.h"

#include <node.h>

piranha::IrValue::IrValue(piranha::IrValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::IrValue::~IrValue() {
	/* void */
}

bool piranha::IrValue::isGeneric() const {
	return (m_type == GENERIC);
}

piranha::NodeOutput *piranha::IrValue::generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	Node *node = program->getRules()->getCachedInstance(this, context);

	if (node == nullptr) node = generateNode(context, program);
	if (node != nullptr) return node->getPrimaryOutput();
	else return nullptr;
}

piranha::Node *piranha::IrValue::generateNode(IrContextTree *context, NodeProgram *program) {
	Node *node = program->getRules()->getCachedInstance(this, context);

	if (node == nullptr) return _generateNode(context, program);
	else return node;
}

piranha::NodeOutput *piranha::IrValue::_generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	return nullptr;
}

piranha::Node *piranha::IrValue::_generateNode(IrContextTree *context, NodeProgram *program) {
	return nullptr;
}
