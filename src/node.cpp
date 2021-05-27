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
    m_evaluating = false;
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

    m_isVirtual = false;

    m_runtimeError = false;

    m_optimizedNode = nullptr;

    m_dead = false;
    m_memorySpace = MemorySpace::PiranhaInternal;
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

bool piranha::Node::evaluate() {
    if (isEvaluated() || m_evaluating) return true;
    else m_evaluating = true;

    const bool status = checkEnabled();
    if (!status) return false;
    if (!isEnabled()) return true;
    
    // First evaluate all dependencies
    const int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        pNodeInput *node = m_inputs[i].input;
        if (node != nullptr && *node != nullptr) {
            // Evaluate the dependency first if it exists
            if (m_inputs[i].dependency != nullptr) {
                const bool result = m_inputs[i].dependency->evaluate();
                if (!result) return false;
            }

            const bool result = (*node)->evaluate();
            if (!result) return false;
        }
    }

    // Node can now self-evaluate
    _evaluate();
    if (m_runtimeError) return false;

    const int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        NodeOutput *output = (*m_outputReferences[i].output);
        if (output != nullptr) {
            const bool result = output->evaluate();
            if (!result) return false;
        }

        Node *nodeOutput = m_outputReferences[i].nodeOutput;
        if (nodeOutput != nullptr) {
            const bool result = nodeOutput->evaluate();
            if (!result) return false;
        }
    }

    m_evaluated = true;
    m_evaluating = false;

    return true;
}

void piranha::Node::destroy() {
    m_initialized = false;
    m_evaluated = false;

    _destroy();
}

void piranha::Node::throwError(const std::string errorMessage) {
    m_runtimeError = true;
    m_program->throwRuntimeError(errorMessage, this);
}

void piranha::Node::connectEnableInput(pNodeInput input, Node *dependency) {
    *m_enableInput = input;
    m_enableInputDependency = dependency;
}

void piranha::Node::connectInput(pNodeInput input, const std::string &name, Node *dependency, Node *nodeInput) {
    const int inputCount = getInputCount();

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
    m_inputs[index].nodeInput = nodeInput;

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

    const int inputCount = getInputCount();
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
    if (name.empty()) return nullptr;

    PortSkeleton *skeleton = getSkeleton(name);
    if (skeleton == nullptr) {
        // This must be an interface node
        return getNodeOutput(name);
    }

    Node *nodeOutput = (skeleton->output != nullptr)
        ? skeleton->output->getInterface()
        : nullptr;
    if (nodeOutput != nullptr) return nodeOutput;

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

    return newNode;
}

piranha::NodeOutput *piranha::Node::generateOutput(const std::string &name) {
    if (name.empty()) return nullptr;

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

piranha::Node *piranha::Node::generateAliasNode() {
    NodeOutput *primary = generateOutput(m_primaryOutput);
    if (primary != nullptr) {
        Node *outInterface = primary->getInterface();
        if (outInterface != nullptr) {
            return outInterface->generateAliasNode();
        }
        else return this;
    }

    Node *primaryNode = generateNodeOutput(m_primaryOutput);
    if (primaryNode != nullptr) {
        return primaryNode->generateAliasNode();
    }

    return this;
}

piranha::NodeOutput *piranha::Node::generateAliasOutput() {
    NodeOutput *primary = generateOutput(m_primaryOutput);
    if (primary != nullptr) {
        Node *outInterface = primary->getInterface();
        if (outInterface != nullptr) {
            return outInterface->generateAliasOutput();
        }
        else return primary;
    }

    Node *primaryNode = generateNodeOutput(m_primaryOutput);
    if (primaryNode != nullptr) {
        return primaryNode->generateAliasOutput();
    }

    return getInterfaceInput();
}

piranha::NodeOutput *piranha::Node::getPrimaryOutput() const {
    return getOutput(m_primaryOutput);
}

piranha::Node *piranha::Node::getPrimaryNode() const {
    return getNodeOutput(m_primaryOutput);
}

piranha::NodeOutput *piranha::Node::getOutput(const std::string &name) const {
    const int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            return m_outputs[i].output;
        }
    }

    const int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        if (name == m_outputReferences[i].name) {
            return *m_outputReferences[i].output;
        }
    }

    const int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        if (name == m_inputs[i].name) {
            return *m_inputs[i].input;
        }
    }

    return nullptr;
}

piranha::Node *piranha::Node::getNodeOutput(const std::string &name) const {
    const int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        if (name == m_outputReferences[i].name) {
            if (m_outputReferences[i].nodeOutput == nullptr) {
                return (*m_outputReferences[i].output)->getInterface();
            }
            else return m_outputReferences[i].nodeOutput;
        }
    }

    const int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            return m_outputs[i].output->getInterface();
        }
    }

    const int inputCount = getInputCount();
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
    const int referenceOutputCount = getOutputReferenceCount();
    for (int i = 0; i < referenceOutputCount; i++) {
        if (name == m_outputReferences[i].name) {
            found = true;
        }
    }

    const int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (name == m_outputs[i].name) {
            found = true;
        }
    }

    return found;
}

std::string piranha::Node::getOutputName(NodeOutput *output) const {
    const int outputCount = getOutputCount();
    for (int i = 0; i < outputCount; i++) {
        if (m_outputs[i].output == output) {
            return m_outputs[i].name;
        }
    }

    const int outputReferenceCount = getOutputReferenceCount();
    for (int i = 0; i < outputReferenceCount; i++) {
        if (*m_outputReferences[i].output == output) {
            return m_outputReferences[i].name;
        }
    }

    const int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        if (*m_inputs[i].input == output) {
            return m_inputs[i].name;
        }
    }

    return "";
}

void piranha::Node::mapOptimizedPort(
    Node *optimizedNode, const std::string &localName, const std::string &mappedName) const 
{
    std::string originalPortName = getMappedPort(mappedName);
    optimizedNode->addPortMapping(localName, originalPortName);
}

void piranha::Node::addPortMapping(const std::string &localName, const std::string &mappedName) {
    PortMapping newMapping = { localName, mappedName };
    m_portMapping.push_back(newMapping);
}

std::string piranha::Node::getLocalPort(const std::string &mappedName) const {
    const int portMappingCount = getPortMappingCount();
    for (int i = 0; i < portMappingCount; i++) {
        if (m_portMapping[i].originalPort == mappedName) {
            return m_portMapping[i].localPort;
        }
    }

    return mappedName;
}

std::string piranha::Node::getMappedPort(const std::string &localName) const {
    const int portMappingCount = getPortMappingCount();
    for (int i = 0; i < portMappingCount; i++) {
        if (m_portMapping[i].localPort == localName) {
            return m_portMapping[i].localPort;
        }
    }

    return localName;
}

void piranha::Node::addFlag(const std::string &name, int data) {
    m_flags.push_back({ name, data });
}

bool piranha::Node::hasFlag(const std::string &name) const {
    const int flagCount = getFlagCount();
    for (int i = 0; i < flagCount; i++) {
        if (m_flags[i].name == name) {
            return true;
        }
    }

    return false;
}

bool piranha::Node::getFlag(const std::string &name, int *dataTarget) const {
    const int flagCount = getFlagCount();
    for (int i = 0; i < flagCount; i++) {
        if (m_flags[i].name == name) {
            if (dataTarget != nullptr) {
                *dataTarget = m_flags[i].data;
            }

            return true;
        }
    }

    return false;
}

piranha::NodeOutput *piranha::Node::getInterfaceInput() const {
    if (m_interfaceInput == nullptr) return nullptr;
    else return *m_interfaceInput;
}

void piranha::Node::writeAssembly(std::fstream &file, Assembly *assembly, int indent) const {
    std::string prefixIndent = "";
    for (int i = 0; i < indent; i++) prefixIndent += "  ";

    const std::string builtinName = getBuiltinName();
    const std::string name = getName();
    
    if (name.empty()) {
        file << prefixIndent + builtinName << "\n";
    }
    else {
        file << prefixIndent + builtinName << ": " << name << "\n";
    }

    const int nodeInputs = getInputCount();
    if (nodeInputs > 0) {
        file << prefixIndent + "  in  { ";
        for (int i = 0; i < nodeInputs; i++) {
            const Node::NodeInputPort *port = getInput(i);
            const int index = assembly->getOutputLabel(*port->input);

            file << port->name << ": &" << index << "; ";
        }
        file << "}\n";
    }

    const int nodeOutputs = getOutputCount();
    const int nodeOutputReferences = getOutputReferenceCount();

    if (nodeOutputReferences > 0 || nodeOutputs > 0) {
        file << prefixIndent + "  out { ";
        for (int i = 0; i < nodeOutputs; i++) {
            const Node::NodeOutputPort *port = getOutput(i);
            const int index = assembly->getOutputLabel(port->output);

            file << port->name << ": @" << index << "; ";
        }

        for (int i = 0; i < nodeOutputReferences; i++) {
            const Node::NodeOutputPortReference *port = getOutputReference(i);
            const int index = assembly->getOutputLabel(*port->output);

            file << port->name << ": &" << index << "; ";
        }
        file << "}\n";
    }
}

piranha::Node *piranha::Node::optimize(NodeAllocator *allocator) {
    if (isOptimized()) return m_optimizedNode;

    // This should never do anything but if the user tries something strange
    // like evaluating the node before optimizing, the optimization step wouldn't break
    m_evaluated = false;

    const int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        pNodeInput input = *m_inputs[i].input;
        Node *nodeInput = m_inputs[i].nodeInput;

        if (input != nullptr) {
            Node *node = input->getParentNode();
            Node *optimizedNode = node->optimize(allocator);

            if (optimizedNode == nullptr) return nullptr; // There was an error
            else if (optimizedNode == node) { /* No optimizations found */ }
            else {
                const std::string name = node->getOutputName(input);
                *m_inputs[i].input = optimizedNode->getOutput(optimizedNode->getLocalPort(name));
                m_inputs[i].dependency = optimizedNode;
                m_inputs[i].nodeInput = optimizedNode;
            }
        }

        if (nodeInput != nullptr) {
            Node *optimizedNode = nodeInput->optimize(allocator);
            if (optimizedNode == nullptr) return nullptr;
            else if (optimizedNode == nodeInput) { /* No optimizations found */ } 
            else {
                m_inputs[i].nodeInput = optimizedNode;
                m_inputs[i].dependency = optimizedNode;
            }
        }
    }

    const int outputCount = getOutputReferenceCount();
    for (int i = 0; i < outputCount; i++) {
        pNodeInput output = *m_outputReferences[i].output;
        Node *nodeOutput = m_outputReferences[i].nodeOutput;

        if (output != nullptr) {
            Node *node = (*m_outputReferences[i].output)->getParentNode();
            Node *optimizedNode = node->optimize(allocator);
            if (optimizedNode == nullptr) return nullptr; // There was an error
            else if (optimizedNode == node) { /* No optimizations found */ }
            else {
                std::string name = node->getOutputName(*m_outputReferences[i].output);
                *m_outputReferences[i].output = optimizedNode->getOutput(optimizedNode->getLocalPort(name));
                m_outputReferences[i].nodeOutput = optimizedNode;
            }
        }

        if (nodeOutput != nullptr) {
            Node *optimizedNode = nodeOutput->optimize(allocator);
            if (optimizedNode == nullptr) return nullptr;
            else if (optimizedNode == nodeOutput) { /* No optimizations found */ }
            else {
                m_outputReferences[i].nodeOutput = optimizedNode;
            }
        }
    }

    Node *optimizedNode = _optimize(allocator);
    if (optimizedNode == nullptr) return nullptr;
    else if (optimizedNode != this) {
        optimizedNode->setMemorySpace(MemorySpace::ClientExternal);

        // Recursively optimize
        optimizedNode->initialize();

        optimizedNode = optimizedNode->optimize(allocator);
        if (optimizedNode == nullptr) return nullptr;
    }

    m_optimizedNode = optimizedNode;

    return optimizedNode;
}

bool piranha::Node::isOptimizedOut() const {
    if (!isOptimized()) return false;
    else if (m_optimizedNode == this) return false;
    else return true;
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

bool piranha::Node::checkEnabled() {
    if (m_checkedEnabled) return true;
    else m_checkedEnabled = true;

    m_enabled = true;

    // Check all dependencies
    const int inputCount = getInputCount();
    for (int i = 0; i < inputCount; i++) {
        // Check the dependency node first
        Node *dependency = m_inputs[i].dependency;
        bool isDependencyDisabled = false;
        if (dependency != nullptr) {
            const bool status = dependency->checkEnabled();
            if (!status) return false;
            if (!dependency->isEnabled()) {
                isDependencyDisabled = true;
                m_enabled = false;
            }
        }

        if (!isDependencyDisabled) {
            pNodeInput *node = m_inputs[i].input;
            if (node != nullptr && *node != nullptr) {
                const bool status = (*node)->checkEnabled();
                if (!status) return false;
                if (!(*node)->isEnabled()) m_enabled = false;
            }
        }
    }

    // Check the enable input
    if (m_enableInput != nullptr) {
        bool isDependencyDisabled = false;
        if (m_enableInputDependency != nullptr) {
            const bool status = m_enableInputDependency->checkEnabled();
            if (!status) return false;
            if (!m_enableInputDependency->isEnabled()) {
                m_enabled = false;
                isDependencyDisabled = true;
            }
        }

        if (!isDependencyDisabled) {
            pNodeInput node = *m_enableInput;

            native_bool enable;
            const bool result = node->evaluate();
            if (!result) return false;
            node->fullCompute((void *)&enable);

            if (!enable) m_enabled = false;
        }
    }

    // Check parent
    if (m_container != nullptr) {
        const bool status = m_container->checkEnabled();
        if (!status) return false;
        if (!m_container->isEnabled()) m_enabled = false;
    }

    return true;
}

void piranha::Node::registerOutputReference(NodeOutput **output, const std::string &name, Node *node) {
    m_outputReferences.push_back({ output, node, name });
}

piranha::Node::PortSkeleton *piranha::Node::getSkeleton(const std::string &name) {
    const int count = (int)m_portSkeletons.size();
    for (int i = 0; i < count; i++) {
        if (m_portSkeletons[i].name == name) {
            return &m_portSkeletons[i];
        }
    }

    return nullptr;
}

piranha::Node *piranha::Node::_optimize(NodeAllocator *allocator) {
    return this;
}

void piranha::Node::setPrimaryOutput(const std::string &name) {
    m_primaryOutput = name;
}

void piranha::Node::registerOutputs() {
    /* void */
}
