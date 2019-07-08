#ifndef PIRANHA_IR_GENERATOR_H
#define PIRANHA_IR_GENERATOR_H

#include <string>
#include <vector>

#include <ir_binary_operator.h>
#include <node_builder.h>

namespace piranha {

	class Node;
	class IrNode;
	class IrContextTree;
	class IrParserStructure;
	class IrBinaryOperator;

	class IrGenerator {
	public:
		IrGenerator();
		~IrGenerator();

		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperator(IrBinaryOperator, IrContextTree *context);

		virtual void registerBuiltinNodeTypes() = 0;

		int getNodeTypeCount() const { return (int)m_nodeBuilders.size(); }
		int getNodeCount() const { return (int)m_nodes.size(); }

	protected:
		Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);
		Node *generateBuiltinType(const std::string &typeName);
		Node *generateCustomType(IrNode *node, IrContextTree *context);

		template <typename NodeType>
		Node *allocateNode() {
			NodeType *newNode = new NodeType;
			m_nodes.push_back(newNode);

			return newNode;
		}

		virtual Node *generateOperator(
			IrBinaryOperator::OPERATOR op, NodeOutput *left, NodeOutput *right) { return nullptr; }

		template <typename NodeType>
		void addBuiltinType(const std::string &builtinName) {
			registerNodeBuilder(
				new SpecializedNodeBuilder<NodeType, builtinName, "">()
			);
		}

	private:
		void registerNodeBuilder(NodeBuilder *builder);

	private:
		std::vector<Node *> m_nodes;
		std::vector<NodeBuilder *> m_nodeBuilders;
	};

} /* namespace piranha */

#endif /* PIRANHA_IR_GENERATOR_H */
