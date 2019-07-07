#include <sdl_value.h>

#include <node.h>

piranha::SdlValue::SdlValue(piranha::SdlValue::VALUE_TYPE type) {
	m_type = type;
}

piranha::SdlValue::~SdlValue() {
	/* void */
}

piranha::NodeOutput *piranha::SdlValue::generateNodeOutput(SdlContextTree *context, NodeProgram *program) {
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

piranha::Node *piranha::SdlValue::generateNode(SdlContextTree *context, NodeProgram *program) {
	GenerationTableEntry *entry = getEntry(context);
	if (entry == nullptr) entry = newEntry(context);

	if (entry->nodeReference == nullptr) {
		entry->nodeReference = _generateNode(context, program);
		return entry->nodeReference;
	}
	else return entry->nodeReference;
}

piranha::NodeOutput *piranha::SdlValue::_generateNodeOutput(SdlContextTree *context, NodeProgram *program) {
	return nullptr;
}

piranha::Node *piranha::SdlValue::_generateNode(SdlContextTree *context, NodeProgram *program) {
	return nullptr;
}

piranha::SdlValue::GenerationTableEntry *piranha::SdlValue::getEntry(SdlContextTree *context) {
	int entryCount = (int)m_generationTable.size();
	for (int i = 0; i < entryCount; i++) {
		if (m_generationTable[i].context == context) {
			return &m_generationTable[i];
		}
	}

	return nullptr;
}

piranha::SdlValue::GenerationTableEntry *piranha::SdlValue::newEntry(SdlContextTree *context) {
	GenerationTableEntry *newEntry = new GenerationTableEntry();
	newEntry->context = context;
	newEntry->nodeGeneratedOutput = nullptr;
	newEntry->nodeReference = nullptr;

	return newEntry;
}
