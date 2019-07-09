#include "ir_value.h"

#include "node_program.h"
#include "generator.h"

#include <node.h>

piranha::IrValue::IrValue(piranha::IrValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::IrValue::~IrValue() {
	/* void */
}

piranha::NodeOutput *piranha::IrValue::generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	Node *node = program->getGenerator()->getCachedInstance(this, context);

	if (node == nullptr) node = generateNode(context, program);
	if (node != nullptr) return node->getPrimaryOutput();
	else return nullptr;
}

piranha::Node *piranha::IrValue::generateNode(IrContextTree *context, NodeProgram *program) {
	Node *node = program->getGenerator()->getCachedInstance(this, context);

	if (node == nullptr) return _generateNode(context, program);
	else return node;
}

piranha::NodeOutput *piranha::IrValue::_generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	return nullptr;
}

piranha::Node *piranha::IrValue::_generateNode(IrContextTree *context, NodeProgram *program) {
	return nullptr;
}

piranha::IrValue::GenerationTableEntry *piranha::IrValue::getEntry(IrContextTree *context) {
	int entryCount = (int)m_generationTable.size();
	for (int i = 0; i < entryCount; i++) {
		if (m_generationTable[i].context == context) {
			return &m_generationTable[i];
		}
	}

	return nullptr;
}

piranha::IrValue::GenerationTableEntry *piranha::IrValue::newEntry(IrContextTree *context) {
	GenerationTableEntry *newEntry = new GenerationTableEntry();
	newEntry->context = context;
	newEntry->nodeGeneratedOutput = nullptr;
	newEntry->nodeReference = nullptr;

	return newEntry;
}
