#ifndef PIRANHA_LANGUAGE_RULES_H
#define PIRANHA_LANGUAGE_RULES_H

#include "literal_node.h"
#include "ir_binary_operator.h"
#include "ir_value.h"
#include "rule.h"
#include "operation_node.h"
#include "key_value_lookup.h"
#include "channel_type.h"

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
		const ChannelType *startingType;
		const ChannelType *targetType;

		bool operator==(const NodeTypeConversion &ref) const {
			return (startingType == ref.startingType) && (targetType == ref.targetType);
		}
	};

	struct OperatorMapping {
		IrBinaryOperator::OPERATOR op;
		const ChannelType *leftType;
		const ChannelType *rightType;
		bool reversible = true;

		bool operator==(const OperatorMapping &ref) const {
			bool typesMatch = leftType == ref.leftType && rightType == ref.rightType;
			if (!typesMatch && ref.reversible && reversible) {
				typesMatch = (leftType == ref.rightType && rightType == ref.leftType);
			}

			return typesMatch && (op == ref.op);
		}
	};

	struct BuiltinTypeInfo {
		const ChannelType *nodeType;
	};

	struct NullType {};

	typedef Rule<BuiltinTypeInfo, Node> BuiltinRule;
	typedef Rule<NullType, Node> ConversionRule;

	class LanguageRules {
	public:
		LanguageRules();
		~LanguageRules();

		virtual void registerBuiltinNodeTypes() = 0;

		Node *getCachedInstance(IrParserStructure *ir, IrContextTree *context);

		template <typename NativeType>
		LiteralNode<NativeType> *generateLiteral(const NativeType &data) {
			typedef LiteralNode<NativeType> ResolvedLiteralNode;

			LiteralType literalType = LiteralTypeLookup<NativeType>();

			std::string *builtinType = m_literalRules.lookup(literalType);
			if (builtinType == nullptr) return nullptr;

			ResolvedLiteralNode *node = static_cast<ResolvedLiteralNode *>(generateBuiltinType(*builtinType));
			node->setData(data);
			
			return node;
		}

		Node *generateNode(IrNode *node, IrContextTree *context);
		Node *generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context);
		std::string resolveOperatorBuiltinType(IrBinaryOperator::OPERATOR op, const ChannelType *left, const ChannelType *right) const;
		const ChannelType *resolveChannelType(const std::string &builtinName) const;

		template <typename NativeType>
		std::string getLiteralBuiltinName() const { return ""; }
		template<> std::string getLiteralBuiltinName<piranha::native_bool>() const { return *m_literalRules.lookup(LITERAL_BOOL); }
		template<> std::string getLiteralBuiltinName<piranha::native_string>() const  { return *m_literalRules.lookup(LITERAL_STRING); }
		template<> std::string getLiteralBuiltinName<piranha::native_int>() const  { return *m_literalRules.lookup(LITERAL_INT); }
		template<> std::string getLiteralBuiltinName<piranha::native_float>() const { return *m_literalRules.lookup(LITERAL_FLOAT); }

		int getNodeCount() const { return (int)m_nodes.size(); }

		void setNodeProgram(NodeProgram *program) { m_nodeProgram = program; }
		NodeProgram *getNodeProgram() const { return m_nodeProgram; }

		Node *generateConversion(const NodeTypeConversion &conversion);

	protected:
		// Main operator hook
		virtual Node *generateOperator(IrBinaryOperator::OPERATOR op, const ChannelType *left, const ChannelType *right);

	protected:
		Node *generateBuiltinType(const std::string &typeName);

		template <typename NodeType>
		Node *allocateNode() {
			NodeType *newNode = new NodeType();
			m_nodes.push_back(newNode);

			return newNode;
		}

		template <typename NodeType>
		void registerBuiltinType(const std::string &builtinName, const ChannelType *nodeType = nullptr) {
			m_builtinRules.newValue<SpecializedRule<BuiltinTypeInfo, NodeType, Node>>(builtinName)
				->setValue({ nodeType });
		}

		void registerLiteralType(LiteralType literalType, const std::string &builtinType);

		template <typename NodeType>
		void registerConversion(const NodeTypeConversion &conversion) {
			m_conversionRules.newValue<SpecializedRule<NullType, NodeType, Node>>(conversion);
		}

		void registerOperator(const OperatorMapping &op, const std::string &builtinType);

	public:
		template <typename BaseType>
		LiteralNode<BaseType> *generateLiteralInternal() { return nullptr; }

	private:
		std::vector<Node *> m_nodes;

		KeyValueLookup<std::string, BuiltinRule> m_builtinRules;
		KeyValueLookup<NodeTypeConversion, ConversionRule> m_conversionRules;
		KeyValueLookup<OperatorMapping, std::string> m_operatorRules;
		KeyValueLookup<LiteralType, std::string> m_literalRules;

		NodeProgram *m_nodeProgram;
	};

} /* namespace piranha */

#endif /* PIRANHA_LANGUAGE_RULES_H */
