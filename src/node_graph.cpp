#include "../include/node_graph.h"

#include "../include/node_program.h"
#include "../include/standard_allocator.h"
#include "../include/exceptions.h"

piranha::NodeGraph::NodeGraph() {
    /* void */
}

piranha::NodeGraph::~NodeGraph() {
    for (GraphNode *graphNode : m_nodes) {
        StandardAllocator::Global()->free(graphNode);
    }
}

void piranha::NodeGraph::generateNodeGraph(NodeProgram *program) {
    NodeContainer *topLevel = program->getTopLevelContainer();
    generateNodes(topLevel);
    generateConnections();
}

void piranha::NodeGraph::markDeadTree(GraphNode *node) {
    node->node->markDead();
    
    int inputCount = (int)node->inConnections.size();
    for (int i = 0; i < inputCount; i++) {
        GraphNode *input = node->inConnections[i];
        input->liveOutputConnections--;

        if (input->liveOutputConnections == 0) {
            if (input->node->hasFlag(piranha::Node::META_ACTIONLESS)) {
                markDeadTree(input);
            }
        }
    }
}

void piranha::NodeGraph::markDeadNodes() {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        int outConnectionCount = (int)m_nodes[i]->outConnections.size();
        m_nodes[i]->liveOutputConnections = outConnectionCount;
    }

    for (int i = 0; i < nodeCount; i++) {
        int outConnectionCount = (int)m_nodes[i]->outConnections.size();
        if (outConnectionCount == 0 && m_nodes[i]->node->hasFlag(piranha::Node::META_ACTIONLESS)) {
            markDeadTree(m_nodes[i]);
        }
    }
}

piranha::NodeGraph::GraphNode *piranha::NodeGraph::findNode(Node *node) {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        if (m_nodes[i]->node == node) return m_nodes[i];
    }

    return nullptr;
}

void piranha::NodeGraph::createNode(Node *node) {
    GraphNode *preexisting = findNode(node);
    if (preexisting != nullptr) return;
    
    GraphNode *newNode = StandardAllocator::Global()->allocate<GraphNode>();
    newNode->node = node;

    m_nodes.push_back(newNode);
}

void piranha::NodeGraph::generateNodes(NodeContainer *container) {
    createNode(container);

    int childCount = container->getChildCount();
    for (int i = 0; i < childCount; i++) {
        generateNodes(container->getChild(i));
    }

    int nodeCount = container->getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        Node *node = container->getNode(i);
        createNode(node);
    }
}

void piranha::NodeGraph::generateConnections() {
    int nodeCount = getNodeCount();

    for (int i = 0; i < nodeCount; i++) {
        GraphNode *graphNode = m_nodes[i];
        Node *node = graphNode->node;

        int inputCount = node->getInputCount();
        for (int j = 0; j < inputCount; j++) {
            pNodeInput input = *node->getInput(j)->input;
            Node *nodeInput = node->getInput(j)->nodeInput;

            Node *node;
            if (input != nullptr) {
                node = input->getParentNode();
            }
            else if (nodeInput != nullptr) {
                node = nodeInput;
            }
            else {
                throw EmptyPort();
            }
            
            GraphNode *endpoint = findNode(node);

            if (!graphNode->hasInConnection(endpoint)) {
                graphNode->inConnections.push_back(endpoint);
            }

            if (!endpoint->hasOutConnection(graphNode)) {
                endpoint->outConnections.push_back(graphNode);
            }
        }
    }
}

bool piranha::NodeGraph::GraphNode::hasOutConnection(GraphNode *node) {
    int outConCount = (int)outConnections.size();
    for (int i = 0; i < outConCount; i++) {
        if (outConnections[i] == node) return true;
    }

    return false;
}

bool piranha::NodeGraph::GraphNode::hasInConnection(GraphNode *node) {
    int inConCount = (int)inConnections.size();
    for (int i = 0; i < inConCount; i++) {
        if (inConnections[i] == node) return true;
    }

    return false;
}
