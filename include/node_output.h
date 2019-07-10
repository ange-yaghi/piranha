#ifndef PIRANHA_NODE_OUTPUT_H
#define PIRANHA_NODE_OUTPUT_H

#include "node_type.h"

#include <string>
#include <vector>

namespace piranha {

	struct IntersectionPoint;
	class Node;

	class NodeOutput {
	public:
		NodeOutput(const NodeType *singleType);
		virtual ~NodeOutput();

		virtual void fullCompute(void *target) const { /* void */ }
		virtual void registerInputs() {}

		int getInputCount() const { return (int)m_inputs.size(); }
		NodeOutput **getInputConnection(int index) { return m_inputs[index]; }

	public:
		bool isType(const NodeType &type) const { return m_singleType == &type; }
		const NodeType *getType() const { return m_singleType; }

		void initialize();
		void evaluate();

		void setName(const std::string &name) { m_name = name; }
		const std::string &getName() const { return m_name; }

		void setParentNode(Node *parentNode) { m_parentNode = parentNode; }
		Node *getParentNode() const { return m_parentNode; }

		Node *getInterface() const { return m_interface; }

	protected:
		void overrideType(const NodeType *type) { m_singleType = type; }

		virtual Node *generateInterface() { return nullptr; }

	private:
		const NodeType *m_singleType;

		std::string m_name;

		Node *m_parentNode;
		Node *m_interface;

		bool m_evaluated;

	protected:
		virtual void registerInput(NodeOutput **nodeInput) { m_inputs.push_back(nodeInput); }

		std::vector<NodeOutput **> m_inputs;
	};

	// Type to reduce confusion
	typedef NodeOutput * pNodeInput;

} /* namespace piranha */

#endif /* PIRANHA_NODE_OUTPUT_H */
