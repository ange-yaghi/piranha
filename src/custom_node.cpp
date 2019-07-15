#include "../include/custom_node.h"

piranha::CustomNode::CustomNode() {
	/* void */
}

piranha::CustomNode::~CustomNode() {
	/* void */
}

void piranha::CustomNode::addCustomOutput(NodeOutput *output, const std::string &name, bool primary) {
	CustomPort *newPort = new CustomPort();
	*newPort = { output, name, primary };

	m_customOutputs.push_back(newPort);
}

void piranha::CustomNode::addCustomInput(const std::string &name) {
	CustomPort *newPort = new CustomPort();
	*newPort = { nullptr, name, false };

	m_customInputs.push_back(newPort);
}

void piranha::CustomNode::_initialize() {
	int outputCount = getCustomOutputCount();
	for (int i = 0; i < outputCount; i++) {
		m_customOutputs[i]->port->initialize();
	}
}

void piranha::CustomNode::registerInputs() {
	int inputCount = getCustomInputCount();
	for (int i = 0; i < inputCount; i++) {
		registerInput(&m_customInputs[i]->port, m_customInputs[i]->name);
	}
}

void piranha::CustomNode::registerOutputs() {
	int outputCount = getCustomOutputCount();
	for (int i = 0; i < outputCount; i++) {
		registerOutputReference(&m_customOutputs[i]->port, m_customOutputs[i]->name);

		if (m_customOutputs[i]->primary) {
			setPrimaryOutput(m_customOutputs[i]->port);
		}
	}
}
