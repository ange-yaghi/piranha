#include "../include/node_output.h"

#include "../include/node.h"

piranha::NodeOutput::NodeOutput(const ChannelType *singleType) {
    m_singleType = singleType; 
    m_evaluated = false;
    m_checkedEnabled = false;
    m_interface = nullptr;

    m_enabled = true;
}

piranha::NodeOutput::~NodeOutput() {
    /* void */
}

int piranha::NodeOutput::getModifyConnectionCount() const {
    return (int)m_modifyConnections.size();
}

void piranha::NodeOutput::addModifyConnection(Node *output) {
    m_modifyConnections.push_back(output);
}

piranha::Node *piranha::NodeOutput::getModifyConnection(int index) const {
    return m_modifyConnections[index];
}

void piranha::NodeOutput::initialize() {
    registerInputs();

    m_interface = generateInterface();
}

bool piranha::NodeOutput::evaluate() {
    if (m_evaluated) return true;
    else m_evaluated = true;

    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        bool result = (*m_inputs[i])->evaluate();
        if (!result) return false;
    }

    int modifyCount = getModifyConnectionCount();
    for (int i = 0; i < modifyCount; i++) {
        bool result = m_modifyConnections[i]->evaluate();
        if (!result) return false;
    }

    if (m_parentNode != nullptr) {
        bool result = m_parentNode->evaluate();
        if (!result) return false;
    }

    _evaluate();
    return true;
}

bool piranha::NodeOutput::checkEnabled() {
    if (m_checkedEnabled) return true;
    else m_checkedEnabled = true;

    m_enabled = true;

    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        bool status = (*m_inputs[i])->checkEnabled();
        if (!status) return false;
        if (!(*m_inputs[i])->isEnabled()) m_enabled = false;
    }

    int modifyCount = getModifyConnectionCount();
    for (int i = 0; i < modifyCount; i++) {
        bool status = m_modifyConnections[i]->checkEnabled();
        if (!status) return false;
        if (!m_modifyConnections[i]->isEnabled()) m_enabled = false;
    }

    if (m_parentNode != nullptr) {
        bool status = m_parentNode->checkEnabled();
        if (!status) return false;
        if (!m_parentNode->isEnabled()) m_enabled = false;
    }

    return true;
}

void piranha::NodeOutput::_evaluate() {
    /* void */
}
