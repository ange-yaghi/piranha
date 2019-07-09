#ifndef PIRANHA_GENERATOR_H
#define PIRANHA_GENERATOR_H

#include <string>
#include <vector>

#include "literal_node.h"
#include <ir_binary_operator.h>
#include <ir_value.h>
#include <node_builder.h>

namespace piranha {

	class Node;
	class NodeProgram;
	class IrNode;
	class IrContextTree;
	class IrParserStructure;
	class IrBinaryOperator;

	typedef NodeBuilder<std::string, Node> BuiltinBuilder;
	typedef NodeBuilder<IrValue::VALUE_TYPE, LiteralNode> LiteralBuilder;

	class Generator {
	public:
		Generator();
		~Generator();

		Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);

		Node *generateLiteral(IrValue *ir, IrContextTree *context);
		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context);

		virtual void registerBuiltinNodeTypes() = 0;

		int getLiteralTypeCount() const { return (int)m_valueBuilders.size(); }
		int getNodeTypeCount() const { return (int)m_nodeBuilders.size(); }
		int getNodeCount() const { return (int)m_nodes.size(); }

		void setNodeProgram(NodeProgram *program) { m_nodeProgram = program; }
		NodeProgram *getNodeProgram() const { return m_nodeProgram; }

	protected:
		// Main operator hook
		virtual Node *generateOperator(IrBinaryOperator::OPERATOR op, NodeOutput *left, NodeOutput *right) {
			return nullptr;
		}

	protected:
		Node *generateBuiltinType(const std::string &typeName);
		Node *generateLiteral(IrValue::VALUE_TYPE valueType);
		Node *generateCustomType();

		template <typename NodeType>
		Node *allocateNode() {
			NodeType *newNode = new NodeType();
			m_nodes.push_back(newNode);

			return newNode;
		}

		template <typename NodeType>
		void registerBuiltinType(const std::string &builtinName) {
			registerNodeBuilder(
				new SpecializedNodeBuilder<NodeType, Node, std::string>(builtinName, "")
			);
		}

		template <typename NodeType>
		void registerLiteralType(IrValue::VALUE_TYPE valueType) {
			registerLiteralBuilder(
				new SpecializedNodeBuilder<NodeType, LiteralNode, IrValue::VALUE_TYPE>(valueType, "")
			);
		}

	private:
		void registerNodeBuilder(BuiltinBuilder *builder);
		void registerLiteralBuilder(LiteralBuilder *builder);

	private:
		std::vector<Node *> m_nodes;
		std::vector<BuiltinBuilder *> m_nodeBuilders;
		std::vector<LiteralBuilder *> m_valueBuilders;

		NodeProgram *m_nodeProgram;
	};

} /* namespace piranha */

#endif /* PIRANHA_GENERATOR_H */
