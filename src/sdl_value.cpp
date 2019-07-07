#include "ir_value.h"

#include <node.h>

piranha::IrValue::IrValue(piranha::IrValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::IrValue::~IrValue() {
	/* void */
}

piranha::NodeOutput *piranha::IrValue::generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	GenerationTableEntry *entry = getEntry(context);
	if (entry == nullptr) entry = newEntry(context);

	if (entry->nodeReference == nullptr) {
		entry->nodeReference = generateNode(context, program);
	}

	if (entry->nodeReference != nullptr) {
		return entry->nodeReference->getPrimaryOutput();
	}

	if (entry->nodeGeneratedOutput == nullptr) {
		entry->nodeGeneratedOutput = _generateNodeOutput(context, program);
	}

	return entry->nodeGeneratedOutput;
}

piranha::Node *piranha::IrValue::generateNode(IrContextTree *context, NodeProgram *program) {
	GenerationTableEntry *entry = getEntry(context);
	if (entry == nullptr) entry = newEntry(context);

	if (entry->nodeReference == nullptr) {
		entry->nodeReference = _generateNode(context, program);
		return entry->nodeReference;
	}
	else return entry->nodeReference;
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
