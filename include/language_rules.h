#ifndef PIRANHA_LANGUAGE_RULES_H
#define PIRANHA_LANGUAGE_RULES_H

#include "literal_node.h"
#include "ir_binary_operator.h"
#include "ir_value.h"
#include "rule.h"
#include "operation_node.h"

#include <string>
#include <vector>

namespace piranha {

	class Node;
	class NodeProgram;
	class IrNode;
	class IrContextTree;
	class IrParserStructure;
	class IrBinaryOperator;

	struct NodeTypeConversion {
		const NodeType *startingType;
		const NodeType *targetType;

		bool operator==(const NodeTypeConversion &ref) const {
			return (startingType == ref.startingType) && (targetType == ref.targetType);
		}
	};

	struct OperatorMapping {
		IrBinaryOperator::OPERATOR op;
		const NodeType *leftType;
		const NodeType *rightType;

		bool operator==(const OperatorMapping &ref) const {
			bool typesMatch = (leftType == ref.leftType && rightType == ref.rightType) ||
				(leftType == ref.rightType && rightType == ref.leftType);

			return typesMatch && (op == ref.op);
		}
	};

	struct NullType {};

	typedef Rule<std::string, NullType, Node> BuiltinBuilder;
	typedef Rule<NodeTypeConversion, NullType, Node> ConversionBuilder;
	typedef Rule<OperatorMapping, NullType, OperationNode> OperatorBuilder;
	template <typename BaseType> using LiteralBuilder = 
		Rule<NullType, NullType, LiteralNode<BaseType>>;

	class Generator {
	public:
		Generator();
		~Generator();

		Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);

		template <typename BaseType>
		LiteralNode<BaseType> *generateLiteral() { return nullptr; }
		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context);

		virtual void registerBuiltinNodeTypes() = 0;

		int getOperatorTypeCount() const { return (int)m_operatorBuilders.size(); }
		int getNodeTypeCount() const { return (int)m_nodeBuilders.size(); }
		int getConversionTypeCount() const { return (int)m_conversionBuilders.size(); }
		int getNodeCount() const { return (int)m_nodes.size(); }

		void setNodeProgram(NodeProgram *program) { m_nodeProgram = program; }
		NodeProgram *getNodeProgram() const { return m_nodeProgram; }

		Node *generateConversion(const NodeTypeConversion &conversion);

	protected:
		// Main operator hook
		virtual Node *generateOperator(IrBinaryOperator::OPERATOR op, const NodeType *left, const NodeType *right);

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

		template <typename NodeType>
		void registerOperator(const OperatorMapping &op) {
			registerOperatorBuilder(
				new SpecializedNodeBuilder<NodeType, OperationNode, OperatorMapping>(op, "")
			);
		}

	private:
		void registerNodeBuilder(BuiltinBuilder *builder);
		void registerConversionBuilder(ConversionBuilder *builder);
		void registerOperatorBuilder(OperatorBuilder *builder);

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
		IntLiteralNode *generateLiteral<piranha::native_int>() {
			return (m_intBuilder != nullptr)
				? m_intBuilder->buildNode()
				: nullptr;
		}

		template <>
		FloatLiteralNode *generateLiteral<piranha::native_float>() {
			return (m_floatBuilder != nullptr)
				? m_floatBuilder->buildNode()
				: nullptr;
		}

		template <>
		StringLiteralNode *generateLiteral<piranha::native_string>() {
			return (m_stringBuilder != nullptr)
				? m_stringBuilder->buildNode()
				: nullptr;
		}

		template <>
		BoolLiteralNode *generateLiteral<piranha::native_bool>() {
			return (m_boolBuilder != nullptr)
				? m_boolBuilder->buildNode()
				: nullptr;
		}

	private:
		std::vector<Node *> m_nodes;

		std::vector<BuiltinBuilder *> m_nodeBuilders;
		std::vector<ConversionBuilder *> m_conversionBuilders;
		std::vector<OperatorBuilder *> m_operatorBuilders;

		LiteralBuilder<piranha::native_int> *m_intBuilder;
		LiteralBuilder<piranha::native_float> *m_floatBuilder;
		LiteralBuilder<piranha::native_bool> *m_boolBuilder;
		LiteralBuilder<piranha::native_string> *m_stringBuilder;

		NodeProgram *m_nodeProgram;
	};

} /* namespace piranha */

#endif /* PIRANHA_LANGUAGE_RULES_H */
