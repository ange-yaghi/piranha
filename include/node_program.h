#ifndef PIRANHA_NODE_PROGRAM_H
#define PIRANHA_NODE_PROGRAM_H

#include <vector>

namespace piranha {

	class Node;
	class Generator;

	class NodeProgram {
	public:
		NodeProgram();
		~NodeProgram();

		void execute();

		int getNodeCount() const { return (int)m_nodes.size();  }
		void addNode(Node *node);
		Node *getNode(int index) const { return m_nodes[index]; }

		void setGenerator(Generator *generator) { m_generator = generator; }
		Generator *getGenerator() { return m_generator; }

	protected:
		std::vector<Node *> m_nodes;

		Generator *m_generator;
	};

} /* namespace piranha */

#endif /* PIRANHA_NODE_PROGRAM_H */
