#ifndef PIRANHA_LANGUAGE_RULES_H
#define PIRANHA_LANGUAGE_RULES_H

#include "literal_node.h"
#include "ir_binary_operator.h"
#include "ir_value.h"
#include "rule.h"
#include "operation_node.h"
#include "key_value_lookup.h"

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

	struct OperatorInfo {
		std::string builtinName;
	};

	struct BuiltinTypeInfo {
		const NodeType *nodeType;
	};

	struct NullType {};

	typedef Rule<BuiltinTypeInfo, Node> BuiltinRule;
	typedef Rule<NullType, Node> ConversionRule;
	typedef Rule<OperatorInfo, OperationNode> OperatorRule;
	template <typename BaseType> using LiteralRule = 
		Rule<NullType, LiteralNode<BaseType>>;

	class LanguageRules {
	public:
		LanguageRules();
		~LanguageRules();

		Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);

		template <typename BaseType>
		LiteralNode<BaseType> *generateLiteral(const BaseType &data) { 
			LiteralNode<BaseType> *node = generateLiteralInternal<BaseType>();
			node->setData(data);
			
			return node;
		}

		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context);
		std::string resolveOperatorBuiltinType(IrBinaryOperator::OPERATOR op, const NodeType *left, const NodeType *right) const;

		virtual void registerBuiltinNodeTypes() = 0;

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
		void registerBuiltinType(const std::string &builtinName, const NodeType *nodeType = nullptr) {
			auto newRule = m_builtinRules.newValue<SpecializedRule<BuiltinTypeInfo, NodeType, Node>>(builtinName);
			newRule->nodeType = nodeType;
		}

		template <typename NodeType, typename LiteralType>
		void registerLiteralType() {
			registerLiteralTypeInternal<NodeType>(LiteralType());
		}

		template <typename NodeType>
		void registerConversion(const NodeTypeConversion &conversion) {
			auto newRule = m_conversionRules.newValue<SpecializedRule<NullType, NodeType, Node>>(conversion);
		}

		template <typename NodeType>
		void registerOperator(const OperatorMapping &op, const OperatorInfo &info) {
			auto newRule = m_operatorRules.newValue<SpecializedRule<OperatorInfo, NodeType, OperationNode>>(op);
			newRule->setValue(info);
		}

	private:
		template <typename NodeType, typename InvalidType>
		void registerLiteralTypeInternal(const InvalidType &otherType) {
			static_assert(false, "Invalid literal type specified; valid options are: LiteralFloatType, LiteralIntType, LiteralBoolType and LiteralStringType");
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralFloatType &) {
			m_floatBuilder = new SpecializedRule<
				NullType, 
				NodeType,
				FloatLiteralNode>();
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralIntType &) {
			m_intBuilder = new SpecializedRule <
				NullType,
				NodeType,
				IntLiteralNode>();
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralBoolType &) {
			m_boolBuilder = new SpecializedRule<
				NullType, 
				NodeType,
				BoolLiteralNode>();
		}

		template <typename NodeType>
		void registerLiteralTypeInternal(const LiteralStringType &) {
			m_stringBuilder = new SpecializedRule<
				NullType, 
				NodeType,
				StringLiteralNode>();
		}

	public:
		template <typename BaseType>
		LiteralNode<BaseType> *generateLiteralInternal() { return nullptr; }

		template <>
		IntLiteralNode *generateLiteralInternal<piranha::native_int>() {
			if (m_intBuilder == nullptr) return nullptr;
			else return m_intBuilder->buildNode();
		}

		template <>
		FloatLiteralNode *generateLiteralInternal<piranha::native_float>() {
			if (m_floatBuilder == nullptr) return nullptr;
			else return m_floatBuilder->buildNode();
		}

		template <>
		StringLiteralNode *generateLiteralInternal<piranha::native_string>() {
			if (m_stringBuilder == nullptr) return nullptr;
			else return m_stringBuilder->buildNode();
		}

		template <>
		BoolLiteralNode *generateLiteralInternal<piranha::native_bool>() {
			if (m_boolBuilder == nullptr) return nullptr;
			else return m_boolBuilder->buildNode();
		}

	private:
		std::vector<Node *> m_nodes;

		KeyValueLookup<std::string, BuiltinRule> m_builtinRules;
		KeyValueLookup<NodeTypeConversion, ConversionRule> m_conversionRules;
		KeyValueLookup<OperatorMapping, OperatorRule> m_operatorRules;

		LiteralRule<piranha::native_int> *m_intBuilder;
		LiteralRule<piranha::native_float> *m_floatBuilder;
		LiteralRule<piranha::native_bool> *m_boolBuilder;
		LiteralRule<piranha::native_string> *m_stringBuilder;

		NodeProgram *m_nodeProgram;
	};

} /* namespace piranha */

#endif /* PIRANHA_LANGUAGE_RULES_H */
