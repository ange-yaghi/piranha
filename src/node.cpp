#include "../include/node.h"

#include "../include/node_output.h"
#include "../include/node_program.h"
#include "../include/fundamental_types.h"
#include "../include/node_container.h"
#include "../include/assembly.h"
#include "../include/ir_attribute_definition.h"

#include <assert.h>

piranha::Node::Node() {
    m_id = -1;
    m_name = "";

    m_initialized = false;
    m_evaluated = false;
    m_primaryOutput = "";

    m_enableInput = nullptr;
    m_enabled = true;
    m_checkedEnabled = false;
    m_enableInputDependency = nullptr;

    m_program = nullptr;
    m_container = nullptr;
    m_interfaceInput = nullptr;

    m_context = nullptr;
    m_irStructure = nullptr;
}

piranha::Node::~Node() {
    /* void */
}

piranha::Node *piranha::Node::getAliasNode() {
    NodeOutput *primary = getPrimaryOutput();
    if (primary != nullptr) {
        Node *outInterface = primary->getInterface();
        if (outInterface != nullptr) {
            return outInterface->getAliasNode();
        }
        else return this;
    }

    Node *primaryNode = getPrimaryNode();
    if (primaryNode != nullptr) {
        return primaryNode->getAliasNode();
    }
    
    return this;
}

piranha::NodeOutput *piranha::Node::getAliasOutput() {
    NodeOutput *primary = getPrimaryOutput();
    if (primary != nullptr) {
        Node *outInterface = primary->getInterface();
        if (outInterface != nullptr) {
            return outInterface->getAliasOutput();
        }
        else return primary;
    }

    Node *primaryNode = getPrimaryNode();
    if (primaryNode != nullptr) {
        return primaryNode->getAliasOutput();
    }

    return getInterfaceInput();
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
    if (!isEnabled()) 
        return;

    // Set evaluated flag
    m_evaluated = true;

    // First evaluate all dependencies
    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        pNodeInput *node = m_inputs[i].input;
        if (node != nullptr && *node != nullptr) {
            // Evaluate the dependency first if it exists
            if (m_inputs[i].dependency != nullptr) {
                m_inputs[i].dependency->evaluate();
            }

            (*node)->evaluate();
        }
    }

    // Node can now self-evaluate
    _evaluate();

    int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        NodeOutput *output = *m_outputReferences[i].output;
        if (output != nullptr) output->evaluate();

        Node *nodeOutput = m_outputReferences[i].nodeOutput;
        if (nodeOutput != nullptr) nodeOutput->evaluate();
    }
}

void piranha::Node::destroy() {
    m_initialized = false;
    m_evaluated = false;

    _destroy();
}

void piranha::Node::connectEnableInput(pNodeInput input, Node *dependency) {
    *m_enableInput = input;
    m_enableInputDependency = dependency;
}

void piranha::Node::connectInput(pNodeInput input, const std::string &name, Node *dependency, Node *nodeInput) {
    int inputCount = getInputCount();

    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            connectInput(input, i, dependency, nodeInput);
            
            // Warning: do not break here! There could potentially be multiple
            // inputs with the same name referencing different endpoints
        }
    }
}

void piranha::Node::connectInput(pNodeInput input, int index, Node *dependency, Node *nodeInput) {
    *m_inputs[index].input = input;
    m_inputs[index].dependency = dependency;

    // If this port can modify the input value, the channel
    // being connected has to be notified
    if (m_inputs[index].modifiesInput) {
        input->addModifyConnection(this);
    }
}

bool piranha::Node::getInputPortInfo(const std::string &name, PortInfo *info) const {
    info->modifiesInput = false;
    info->isToggle = false;
    info->isAlias = false;

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

void piranha::Node::addPortSkeleton(const PortSkeleton &skeleton) {
    m_portSkeletons.push_back(skeleton);
}

piranha::Node *piranha::Node::generateNodeOutput(const std::string &name) {
    PortSkeleton *skeleton = getSkeleton(name);
    if (skeleton == nullptr) {
        // This must be an interface node
        return getNodeOutput(name);
    }

    if (skeleton->nodeOutput != nullptr) return skeleton->nodeOutput;

    NodeContainer *container = skeleton->container;

    Node *newNode = skeleton->definition->generateNode(
        skeleton->context,
        m_program,
        skeleton->container);

    if (newNode != nullptr) {
        if (!container->findContainer(newNode)) {
            container->addNode(newNode);
        }
    }

    skeleton->nodeOutput = newNode;

    return newNode;
}

piranha::NodeOutput *piranha::Node::generateOutput(const std::string &name) {
    PortSkeleton *skeleton = getSkeleton(name);
    if (skeleton == nullptr) {
        // This must be an interface node
        return getOutput(name);
    }

    if (skeleton->output != nullptr) return skeleton->output;

    NodeContainer *container = skeleton->container;

    NodeOutput *newOutput = skeleton->definition->generateNodeOutput(
        skeleton->context,
        m_program,
        skeleton->container);

    if (newOutput != nullptr) {
        Node *nodeInterface = newOutput->getInterface();
        if (nodeInterface != nullptr) {
            container->addNode(nodeInterface);
        }
    }

    skeleton->output = newOutput;

    return newOutput;
}

piranha::NodeOutput *piranha::Node::getPrimaryOutput() const {
    return getOutput(m_primaryOutput);
}

piranha::Node *piranha::Node::getPrimaryNode() const {
    return getNodeOutput(m_primaryOutput);
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

    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            return *m_inputs[i].input;
        }
    }

    return nullptr;
}

piranha::Node *piranha::Node::getNodeOutput(const std::string &name) const {
    int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        if (name == m_outputReferences[i].name) {
            if (m_outputReferences[i].nodeOutput == nullptr) {
                return (*m_outputReferences[i].output)->getInterface();
            }
            else return m_outputReferences[i].nodeOutput;
        }
    }

    int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            return m_outputs[i].output->getInterface();
        }
    }

    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            if (m_inputs[i].nodeInput == nullptr) {
                return (*m_inputs[i].input)->getInterface();
            }
            else return m_inputs[i].nodeInput;
        }
    }

    return nullptr;
}

bool piranha::Node::getOutputPortInfo(const std::string &name, PortInfo *info) const {
    info->modifiesInput = false;
    info->isToggle = false;
    info->isAlias = false;

    if (m_primaryOutput == name) {
        info->isAlias = true;
    }

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

piranha::NodeOutput *piranha::Node::getInterfaceInput() const {
    if (m_interfaceInput == nullptr) return nullptr;
    else return *m_interfaceInput;
}

void piranha::Node::writeAssembly(std::fstream &file, Assembly *assembly) const {
    std::string builtinName = getBuiltinName();

    file << builtinName << "\n";

    int nodeInputs = getInputCount();
    if (nodeInputs > 0) {
        file << "    INPUTS  { ";
        for (int i = 0; i < nodeInputs; i++) {
            const Node::NodeInputPort *port = getInput(i);
            int index = assembly->getOutputLabel(*port->input);

            file << port->name << ": &" << index << "; ";
        }
        file << "}\n";
    }

    int nodeOutputs = getOutputCount();
    int nodeOutputReferences = getOutputReferenceCount();

    if (nodeOutputReferences > 0 || nodeOutputs > 0) {
        file << "    OUTPUTS { ";
        for (int i = 0; i < nodeOutputs; i++) {
            const Node::NodeOutputPort *port = getOutput(i);
            int index = assembly->getOutputLabel(port->output);

            file << port->name << ": @" << index << "; ";
        }

        for (int i = 0; i < nodeOutputReferences; i++) {
            const Node::NodeOutputPortReference *port = getOutputReference(i);
            int index = assembly->getOutputLabel(*port->output);

            file << port->name << ": &" << index << "; ";
        }
        file << "}\n";
    }
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
    m_inputs.push_back({ node, nullptr, nullptr, name, modifiesInput, enableInput });

    if (enableInput) m_enableInput = node;

    // Reset the port
    *node = nullptr;
}

void piranha::Node::registerOutput(NodeOutput *node, const std::string &name) {
    m_outputs.push_back({ node, name });
    node->setParentNode(this);
}

void piranha::Node::checkEnabled() {
    if (m_checkedEnabled) return;
    else m_checkedEnabled = true;

    m_enabled = true;

    // Check all dependencies
    int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        // Check the dependency node first
        Node *dependency = m_inputs[i].dependency;
        bool isDependencyDisabled = false;
        if (dependency != nullptr) {
            dependency->checkEnabled();
            if (!dependency->isEnabled()) {
                isDependencyDisabled = true;
                m_enabled = false;
            }
        }

        if (!isDependencyDisabled) {
            pNodeInput *node = m_inputs[i].input;
            if (node != nullptr && *node != nullptr) {
                (*node)->checkEnabled();
                if (!(*node)->isEnabled()) m_enabled = false;
            }
        }
    }

    // Check the enable input
    if (m_enableInput != nullptr) {
        bool isDependencyDisabled = false;
        if (m_enableInputDependency != nullptr) {
            m_enableInputDependency->checkEnabled();
            if (!m_enableInputDependency->isEnabled()) {
                m_enabled = false;
                isDependencyDisabled = true;
            }
        }

        if (!isDependencyDisabled) {
            pNodeInput node = *m_enableInput;

            native_bool enable;
            node->evaluate();
            node->fullCompute((void *)&enable);

            if (!enable) m_enabled = false;
        }
    }

    // Check parent
    if (m_container != nullptr) {
        m_container->checkEnabled();
        if (!m_container->isEnabled()) m_enabled = false;
    }
}

void piranha::Node::registerOutputReference(NodeOutput *const *output, const std::string &name, Node *node) {
    m_outputReferences.push_back({ output, node, name });
}

piranha::Node::PortSkeleton *piranha::Node::getSkeleton(const std::string &name) {
    int count = (int)m_portSkeletons.size();
    for (int i = 0; i < count; i++) {
        if (m_portSkeletons[i].name == name) {
            return &m_portSkeletons[i];
        }
    }

    return nullptr;
}

void piranha::Node::setPrimaryOutput(const std::string &name) {
    m_primaryOutput = name;
}

void piranha::Node::registerOutputs() {
    /* void */
}
