#include "../include/node_output.h"

#include "../include/node.h"

piranha::NodeOutput::NodeOutput(const ChannelType *singleType) {
    m_singleType = singleType; 
    m_evaluated = false;
    m_interface = nullptr;
}

piranha::NodeOutput::~NodeOutput() {
    /* void */
}

int piranha::NodeOutput::getModifyConnectionCount() const {
	return (int)m_modifyConnections.size();
}

void piranha::NodeOutput::addModifyConnection(NodeOutput *output) {
	m_modifyConnections.push_back(output);
}

piranha::NodeOutput *piranha::NodeOutput::getModifyConnection(int index) const {
	return m_modifyConnections[index];
}

void piranha::NodeOutput::initialize() {
    m_interface = generateInterface();
    registerInputs();
}

void piranha::NodeOutput::evaluate() {
    if (m_evaluated) return;
    m_evaluated = true;

    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        (*m_inputs[i])->evaluate();
    }

	int modifyCount = getModifyConnectionCount();
	for (int i = 0; i < modifyCount; i++) {
		m_modifyConnections[i]->evaluate();
	}

    if (m_parentNode != nullptr) {
        m_parentNode->evaluate();
    }
}
