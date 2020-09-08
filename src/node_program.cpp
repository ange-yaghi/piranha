#include "../include/node_program.h"

#include "../include/node.h"
#include "../include/ir_context_tree.h"
#include "../include/key_value_lookup.h"
#include "../include/ir_node.h"
#include "../include/ir_node_definition.h"
#include "../include/assembly.h"
#include "../include/node_graph.h"

#include <fstream>

piranha::NodeProgram::NodeProgram() {
    m_topLevelContainer.setName("root");

    m_errorMessage = "";
    m_errorNode = nullptr;
    m_runtimeError = false;

    m_initialized = false;
}

piranha::NodeProgram::~NodeProgram() {
    /* void */
}

void piranha::NodeProgram::writeAssembly(const std::string &fname) const {
    std::fstream file(fname, std::ios::out);

    Assembly assembly;
    m_topLevelContainer.writeAssembly(file, &assembly, 0);

    file.close();
}

void piranha::NodeProgram::addNode(Node *node) {
    m_nodeCache.push_back(node);

    node->setProgram(this);
}

void piranha::NodeProgram::addContainer(IrContextTree *context, NodeContainer *container) {
    *m_containers.newValue(context) = container;
}

piranha::NodeContainer *piranha::NodeProgram::getContainer(IrContextTree *context) {
     NodeContainer **container = m_containers.lookup(context);
     if (container == nullptr) return nullptr;
     else return *container;
}

void piranha::NodeProgram::throwRuntimeError(const std::string &msg, Node *node) {
    m_errorMessage = msg;
    m_errorNode = node;
    m_runtimeError = true;
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

void piranha::NodeProgram::initialize() {
    if (m_initialized) return;

    int nodeCount = m_topLevelContainer.getNodeCount();

    // Initialize all nodes
    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        node->initialize();
    }

    m_initialized = true;
}

void piranha::NodeProgram::optimize() {
    m_topLevelContainer.optimize();

    NodeGraph graph;
    graph.generateNodeGraph(this);
    graph.markDeadNodes();

    m_topLevelContainer.prune();
}

bool piranha::NodeProgram::execute() {
    int nodeCount = m_topLevelContainer.getNodeCount();

    // For backward compatibility
    initialize();
    
    // Execute all nodes
    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        bool result = node->evaluate();
        if (!result) return false;
    }

    return true;
}

void piranha::NodeProgram::free() {

}
