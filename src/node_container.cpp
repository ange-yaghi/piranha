#include "../include/node_container.h"

#include "../include/ir_context_tree.h"

piranha::NodeContainer::NodeContainer() {
    /* void */
}

piranha::NodeContainer::~NodeContainer() {
    /* void */
}

void piranha::NodeContainer::addNode(Node *node) {
    if (getTopLevel()->findNode(node)) return;

    if (node->getName() == "settings") {
        int a = 0;
    }

    m_nodes.push_back(node);
    node->setContainer(this);
}

bool piranha::NodeContainer::findNode(Node *node) const {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        if (m_nodes[i] == node) return true;
    }

    int childCount = getChildCount();
    for (int i = 0; i < childCount; i++) {
        if (m_children[i]->findNode(node)) return true;
    }

    return false;
}

void piranha::NodeContainer::addChild(NodeContainer *container) {
    m_children.push_back(container);
}

piranha::NodeContainer *piranha::NodeContainer::getTopLevel() {
    if (m_container != nullptr) return m_container->getTopLevel();
    else return this;
}

void piranha::NodeContainer::addContainerInput(const std::string &name, bool modifiesInput, bool enableInput) {
    pNodeInput *connectionPoint = new pNodeInput;
    m_connections.push_back(connectionPoint);

    registerInput(connectionPoint, name, modifiesInput, enableInput);
}

void piranha::NodeContainer::addContainerOutput(pNodeInput output, Node *node, const std::string &name, bool primary) {
    pNodeInput *connectionPoint = new pNodeInput;
    m_connections.push_back(connectionPoint);

    *connectionPoint = output;

    registerOutputReference(connectionPoint, name, node);

    if (primary) {
        setPrimaryOutput(name);
    }
}

void piranha::NodeContainer::_initialize() {
    /* void */
}

void piranha::NodeContainer::_evaluate() {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        m_nodes[i]->initialize();
    }

    for (int i = 0; i < nodeCount; i++) {
        m_nodes[i]->evaluate();
    }
}

void piranha::NodeContainer::_destroy() {
    /* void */
}
