#ifndef NODE_PROGRAM_H
#define NODE_PROGRAM_H

#include <vector>

namespace piranha {

	class Node;

	class NodeProgram {
	public:
		NodeProgram();
		~NodeProgram();

		int getNodeCount() const { return (int)m_nodes.size();  }
		void addNode(Node *node);
		Node *getNode(int index) const { return m_nodes[index]; }

		void execute();

	protected:
		std::vector<Node *> m_nodes;
	};

} /* namespace piranha */

#endif /* NODE_PROGRAM_H */
