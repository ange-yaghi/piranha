#include "../include/node_program.h"

#include "../include/node.h"
#include "../include/ir_context_tree.h"
#include "../include/key_value_lookup.h"
#include "../include/ir_node.h"
#include "../include/ir_node_definition.h"

#include <fstream>

piranha::NodeProgram::NodeProgram() {
    /* void */
}

piranha::NodeProgram::~NodeProgram() {
    /* void */
}

void piranha::NodeProgram::writeAssembly(const std::string &fname) const {

}

void piranha::NodeProgram::addNode(Node *node) {
    m_nodeCache.push_back(node);
}

void piranha::NodeProgram::addContainer(IrContextTree *context, NodeContainer *container) {
    *m_containers.newValue(context) = container;
}

piranha::NodeContainer *piranha::NodeProgram::getContainer(IrContextTree *context) {
     NodeContainer **container = m_containers.lookup(context);
     if (container == nullptr) return nullptr;
     else return *container;
}

piranha::Node *piranha::NodeProgram::getCachedInstance(IrParserStructure *ir, IrContextTree *context) {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        if (m_nodeCache[i]->getIrStructure() == ir) {
            if (m_nodeCache[i]->getContext()->isEqual(context)) {
                return m_nodeCache[i];
            }
        }
    }

    return nullptr;
}

void piranha::NodeProgram::execute() {
    int nodeCount = m_topLevelContainer.getNodeCount();

    // Initialize all nodes
    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        node->initialize();
    }

    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        node->evaluate();
    }
}
