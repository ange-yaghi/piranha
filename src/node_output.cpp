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

    if (m_parentNode != nullptr) {
        m_parentNode->evaluate();
    }
}
