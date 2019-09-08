#include "../include/node_container.h"

#include "../include/ir_context_tree.h"
#include "../include/key_value_lookup.h"
#include "../include/assembly.h"

piranha::NodeContainer::NodeContainer() {
    m_container = nullptr;
}

piranha::NodeContainer::~NodeContainer() {
    /* void */
}

void piranha::NodeContainer::addNode(Node *node) {
    if (getTopLevel()->findNode(node)) return;

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

bool piranha::NodeContainer::findContainer(Node *node) const {
    if (node == this) return true;
    
    int childCount = getChildCount();
    for (int i = 0; i < childCount; i++) {
        if (m_children[i]->findContainer(node)) return true;
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

void piranha::NodeContainer::
    writeAssembly(std::fstream &file, Assembly *assembly, int indent) const 
{
    std::string prefixIndent = "";
    for (int i = 0; i < indent; i++) prefixIndent += "  ";

    std::string name = getName();
    file << prefixIndent;
    if (name.empty()) file << "{" << std::endl;
    else file << name << " {" << std::endl;

    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        Node *node = getNode(i);
        node->writeAssembly(file, assembly, indent + 1);
    }

    file << prefixIndent << "}" << std::endl;
}

void piranha::NodeContainer::prune() {
    int nodeCount = getNodeCount();
    int newNodeCount = 0;
    for (int i = 0; i < nodeCount; i++) {
        bool optimizedOut = m_nodes[i]->isOptimizedOut() || m_nodes[i]->isDead();

        if (optimizedOut) {
            /* void */
        }
        else {
            m_nodes[newNodeCount++] = m_nodes[i];
        }
    }

    m_nodes.resize(newNodeCount);

    int childCount = getChildCount();
    for (int i = 0; i < childCount; i++) {
        m_children[i]->prune();
    }
}

void piranha::NodeContainer::_initialize() {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        m_nodes[i]->initialize();
    }
}

void piranha::NodeContainer::_evaluate() {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        bool result = m_nodes[i]->evaluate();
        if (!result) return;
    }
}

void piranha::NodeContainer::_destroy() {
    /* void */
}

piranha::Node *piranha::NodeContainer::_optimize() {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        m_nodes[i] = m_nodes[i]->optimize();
    }

    return this;
}