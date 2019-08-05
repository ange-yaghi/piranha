#ifndef PIRANHA_NODE_CONTAINER_H
#define PIRANHA_NODE_CONTAINER_H

#include "node.h"

#include <vector>

namespace piranha {

    class NodeContainer : public Node {
    public:
        NodeContainer();
        virtual ~NodeContainer();

        void addNode(Node *node);
        Node *getNode(int index) const { return m_nodes[index]; }
        int getNodeCount() const { return (int)m_nodes.size(); }

        Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);

        int getChildCount() const { return (int)m_children.size(); }

        NodeContainer *getTopLevel();

        void addContainerInput(const std::string &name, bool modifiesInput, bool enableInput);
        void addContainerOutput(pNodeInput output, Node *node, const std::string &name, bool primary);

    protected:
        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

    protected:
        std::vector<NodeContainer *> m_children;
        std::vector<Node *> m_nodes;

        std::vector<pNodeInput *> m_connections;
    };

} /* namspace piranha */

#endif /* PIRANHA_NODE_CONTAINER_H */
