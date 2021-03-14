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
    
    const int inputCount = (int)node->inConnections.size();
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
    const int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        const int outConnectionCount = (int)m_nodes[i]->outConnections.size();
        m_nodes[i]->liveOutputConnections = outConnectionCount;
    }

    for (int i = 0; i < nodeCount; i++) {
        const int outConnectionCount = (int)m_nodes[i]->outConnections.size();
        if (outConnectionCount == 0 && m_nodes[i]->node->hasFlag(piranha::Node::META_ACTIONLESS)) {
            markDeadTree(m_nodes[i]);
        }
    }
}

piranha::NodeGraph::GraphNode *piranha::NodeGraph::findNode(Node *node) {
    const int nodeCount = getNodeCount();
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

    const int childCount = container->getChildCount();
    for (int i = 0; i < childCount; i++) {
        generateNodes(container->getChild(i));
    }

    const int nodeCount = container->getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        Node *node = container->getNode(i);
        createNode(node);
    }
}

void piranha::NodeGraph::generateConnections() {
    const int nodeCount = getNodeCount();

    for (int i = 0; i < nodeCount; i++) {
        GraphNode *graphNode = m_nodes[i];
        Node *node = graphNode->node;

        const int inputCount = node->getInputCount();
        for (int j = 0; j < inputCount; j++) {
            pNodeInput input = *node->getInput(j)->input;
            Node *nodeInput = node->getInput(j)->nodeInput;
            Node *dependency = node->getInput(j)->dependency;

            std::vector<Node *> toCheck = {
                (input == nullptr) ? nullptr : input->getParentNode(),
                nodeInput,
                dependency
            };

            if (input == nullptr && nodeInput == nullptr) {
                throw EmptyPort();
            }

            for (Node *node : toCheck) {
                if (node != nullptr) {
                    GraphNode *endpoint = findNode(node);
                    if (endpoint == nullptr) continue;

                    if (!graphNode->hasInConnection(endpoint)) {
                        graphNode->inConnections.push_back(endpoint);
                    }

                    if (!endpoint->hasOutConnection(graphNode)) {
                        endpoint->outConnections.push_back(graphNode);
                    }
                }
            }
        }
    }
}

bool piranha::NodeGraph::GraphNode::hasOutConnection(GraphNode *node) {
    const int outConCount = (int)outConnections.size();
    for (int i = 0; i < outConCount; i++) {
        if (outConnections[i] == node) return true;
    }

    return false;
}

bool piranha::NodeGraph::GraphNode::hasInConnection(GraphNode *node) {
    const int inConCount = (int)inConnections.size();
    for (int i = 0; i < inConCount; i++) {
        if (inConnections[i] == node) return true;
    }

    return false;
}
