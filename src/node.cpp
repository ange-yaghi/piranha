#include "../include/node.h"

#include "../include/node_output.h"
#include "../include/node_program.h"
#include "../include/fundamental_types.h"

#include <assert.h>

piranha::Node::Node() {
    m_id = -1;
    m_name = "";

    m_initialized = false;
    m_evaluated = false;
    m_primaryOutput = nullptr;
    m_primaryReference = nullptr;

    m_enableInput = nullptr;
    m_enabled = true;
}

piranha::Node::~Node() {
    /* void */
}

void piranha::Node::initialize() {
    if (isInitialized()) return;

    // Set initialized flag
    m_initialized = true;

    // Initialize
    _initialize();

    registerInputs();
    registerOutputs();
}

void piranha::Node::evaluate() {
    if (isEvaluated()) return;

    checkEnabled();
    if (!isEnabled()) return;

    // Set evaluated flag
    m_evaluated = true;

    // First evaluate all dependencies
    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        pNodeInput *node = m_inputs[i].input;
        if (node != nullptr && *node != nullptr) {
            (*node)->evaluate();
        }
    }

    // Node can now self-evaluate
    _evaluate();

    int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        NodeOutput *node = *m_outputReferences[i].output;
        node->evaluate();
    }
}

void piranha::Node::destroy() {
    m_initialized = false;
    m_evaluated = false;

    _destroy();
}

void piranha::Node::connectInput(pNodeInput input, const std::string &name) {
    int inputCount = getInputCount();

    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            connectInput(input, i);
            
            // Warning: do not break here! There could potentially be multiple
            // inputs with the same name referencing different endpoints
        }
    }
}

void piranha::Node::connectInput(pNodeInput input, int index) {
    *m_inputs[index].input = input;

    // If this port can modify the input value, the channel
    // being connected has to be notified
    if (m_inputs[index].modifiesInput) {
        input->addModifyConnection(this);
    }
}

void piranha::Node::connectDefaultInput(pNodeInput input) {
    assert(getInputCount() == 1); // Use of this function is reserved with single input nodes

    connectInput(input, 0);
}

bool piranha::Node::getInputPortInfo(const std::string &name, PortInfo *info) const {
    info->modifiesInput = false;
    info->isToggle = false;

    int inputCount = getInputCount();
    bool found = false;
    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            info->modifiesInput = m_inputs[i].modifiesInput || info->modifiesInput;
            info->isToggle = m_inputs[i].enableInput || info->isToggle;
            found = true;
        }
    }

    return found;
}

piranha::NodeOutput *piranha::Node::getPrimaryOutput() const {
    return (m_primaryOutput != nullptr) 
        ? m_primaryOutput
        : (m_primaryReference != nullptr) 
            ? *m_primaryReference
            : nullptr;
}

piranha::NodeOutput *piranha::Node::getOutput(const std::string &name) const {
    int outputCount = getOutputCount();

    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            return m_outputs[i].output;
        }
    }

    int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        if (name == m_outputReferences[i].name) {
            return *m_outputReferences[i].output;
        }
    }

    return nullptr;
}

bool piranha::Node::getOutputPortInfo(const std::string &name, PortInfo *info) const {
    info->modifiesInput = false;

    bool found = false;
    int referenceOutputCount = getOutputReferenceCount();
    for (int i = 0; i < referenceOutputCount; i++) {
        if (name == m_outputReferences[i].name) {
            found = true;
        }
    }

    int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            found = true;
        }
    }

    return found;
}

void piranha::Node::_initialize() {
    /* void */
}

void piranha::Node::_evaluate() {
    /* void */
}

void piranha::Node::_destroy() {
    /* void */
}

void piranha::Node::registerInputs() {
    /* void */
}

void piranha::Node::
    registerInput(pNodeInput *node, const std::string &name, bool modifiesInput, bool enableInput)
{
    m_inputs.push_back({ node, name, modifiesInput, enableInput });

    if (enableInput) m_enableInput = node;
}

void piranha::Node::registerOutput(NodeOutput *node, const std::string &name) {
    m_outputs.push_back({ node, name });
    node->setParentNode(this);
}

void piranha::Node::setPrimaryOutput(NodeOutput *node) {
    m_primaryOutput = node;
}

void piranha::Node::setPrimaryOutputReference(NodeOutput **node) {
    m_primaryReference = node;
}

void piranha::Node::checkEnabled() {
    if (m_checkedEnabled) return;
    else m_checkedEnabled = true;

    bool enabled = true;

    // Check all dependencies
    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        pNodeInput *node = m_inputs[i].input;
        if (node != nullptr && *node != nullptr) {
            (*node)->checkEnabled();
            if (!(*node)->isEnabled()) enabled = false;
        }
    }

    // Check the enable input
    if (m_enableInput != nullptr) {
        pNodeInput node = *m_enableInput;

        native_bool enable;
        node->fullCompute((void *)&enable);

        if (!enable) enabled = false;
    }

    m_enabled = enabled;
}

void piranha::Node::registerOutputReference(NodeOutput *const *node, const std::string &name) {
    m_outputReferences.push_back({ node, name });
}

void piranha::Node::registerOutputs() {
    /* void */
}
