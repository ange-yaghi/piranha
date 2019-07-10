#ifndef PIRANHA_GENERATOR_H
#define PIRANHA_GENERATOR_H

#include "literal_node.h"
#include "ir_binary_operator.h"
#include "ir_value.h"
#include "node_builder.h"

#include <string>
#include <vector>

namespace piranha {

	class Node;
	class NodeProgram;
	class IrNode;
	class IrContextTree;
	class ParserStructure;
	class IrBinaryOperator;

	struct NodeTypeConversion {
		const NodeType *startingType;
		const NodeType *targetType;

		bool operator==(const NodeTypeConversion &ref) const {
			return (startingType == ref.startingType) && (targetType == ref.targetType);
		}
	};

	typedef NodeBuilder<std::string, Node> BuiltinBuilder;
	typedef NodeBuilder<NodeTypeConversion, Node> ConversionBuilder;
	template <typename BaseType> using LiteralBuilder = 
		NodeBuilder<int, LiteralNode<BaseType>>;

	class Generator {
	public:
		Generator();
		~Generator();

		Node *getCachedInstance(ParserStructure *ir, IrContextTree *context);

		template <typename BaseType>
		LiteralNode<BaseType> *generateLiteral() { return nullptr; }
		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context);

		virtual void registerBuiltinNodeTypes() = 0;

		int getNodeTypeCount() const { return (int)m_nodeBuilders.size(); }
		int getConversionTypeCount() const { return (int)m_conversionBuilders.size(); }
		int getNodeCount() const { return (int)m_nodes.size(); }

		void setNodeProgram(NodeProgram *program) { m_nodeProgram = program; }
		NodeProgram *getNodeProgram() const { return m_nodeProgram; }

		Node *generateConversion(const NodeTypeConversion &conversion);

	protected:
		// Main operator hook
		virtual Node *generateOperator(IrBinaryOperator::OPERATOR op, NodeOutput *left, NodeOutput *right) {
			return nullptr;
		}

	protected:
		Node *generateBuiltinType(const std::string &typeName);
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

		template <typename NodeType, typename LiteralType>
		void registerLiteralType() {
			registerLiteralTypeInternal<NodeType>(LiteralType());
		}

		template <typename NodeType>
		void registerConversion(const NodeTypeConversion &conversion) {
			registerConversionBuilder(
				new SpecializedNodeBuilder<NodeType, Node, NodeTypeConversion>(conversion, "")
			);
		}

	private:
		void registerNodeBuilder(BuiltinBuilder *builder);
		void registerConversionBuilder(ConversionBuilder *builder);

	private:
		template <typename NodeType, typename InvalidType>
		void registerLiteralTypeInternal(const InvalidType &otherType) {
			static_assert(false, "Invalid literal type specified; valid options are: LiteralFloatType, LiteralIntType, LiteralBoolType and LiteralStringType");
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralFloatType &) {
			m_floatBuilder = new SpecializedNodeBuilder<
				NodeType, 
				FloatLiteralNode,
				int>(0, "");
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralIntType &) {
			m_intBuilder = new SpecializedNodeBuilder<
				NodeType, 
				IntLiteralNode,
				int>(0, "");
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralBoolType &) {
			m_boolBuilder = new SpecializedNodeBuilder<
				NodeType, 
				BoolLiteralNode,
				int>(0, "");
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralStringType &) {
			m_stringBuilder = new SpecializedNodeBuilder<
				NodeType, 
				StringLiteralNode,
				int>(0, "");
		}

	public:
		template <>
		IntLiteralNode *generateLiteral<piranha::literal_int>() {
			return (m_intBuilder != nullptr)
				? m_intBuilder->buildNode()
				: nullptr;
		}

		template <>
		FloatLiteralNode *generateLiteral<piranha::literal_float>() {
			return (m_floatBuilder != nullptr)
				? m_floatBuilder->buildNode()
				: nullptr;
		}

		template <>
		StringLiteralNode *generateLiteral<piranha::literal_string>() {
			return (m_stringBuilder != nullptr)
				? m_stringBuilder->buildNode()
				: nullptr;
		}

		template <>
		BoolLiteralNode *generateLiteral<piranha::literal_bool>() {
			return (m_boolBuilder != nullptr)
				? m_boolBuilder->buildNode()
				: nullptr;
		}

	private:
		std::vector<Node *> m_nodes;

		std::vector<BuiltinBuilder *> m_nodeBuilders;
		std::vector<ConversionBuilder *> m_conversionBuilders;

		LiteralBuilder<piranha::literal_int> *m_intBuilder;
		LiteralBuilder<piranha::literal_float> *m_floatBuilder;
		LiteralBuilder<piranha::literal_bool> *m_boolBuilder;
		LiteralBuilder<piranha::literal_string> *m_stringBuilder;

		NodeProgram *m_nodeProgram;
	};

} /* namespace piranha */

#endif /* PIRANHA_GENERATOR_H */
