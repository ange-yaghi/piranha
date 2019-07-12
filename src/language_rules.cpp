#include "language_rules.h"

#include "rule.h"
#include "node.h"
#include "custom_node.h"
#include "ir_node.h"
#include "ir_context_tree.h"
#include "ir_node_definition.h"
#include "ir_value_constant.h"

#include <assert.h>

piranha::LanguageRules::LanguageRules() {
	/* void */
}

piranha::LanguageRules::~LanguageRules() {
	/* void */
}

piranha::Node *piranha::LanguageRules::generateNode(IrNode *node, IrContextTree *context) {
	Node *cachedNode = getCachedInstance(node, context);
	if (cachedNode != nullptr) return cachedNode;
	else {
		IrNodeDefinition *definition = node->getDefinition();
		if (definition == nullptr) return nullptr;

		Node *newNode = nullptr;
		if (definition->isBuiltin()) {
			newNode = generateBuiltinType(definition->getBuiltinName());
		}
		else newNode = generateCustomType();

		newNode->setIrContext(context);
		newNode->setIrStructure(node);

		return newNode;
	}
}

piranha::Node *piranha::LanguageRules::generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context) {
	IrValue *left = binOp->getLeft();
	IrValue *right = binOp->getRight();

	NodeOutput *leftNodeOutput = left->generateNodeOutput(context, m_nodeProgram);
	NodeOutput *rightNodeOutput = right->generateNodeOutput(context, m_nodeProgram);
	
	Node *node = generateOperator(binOp->getOperator(), leftNodeOutput->getType(), rightNodeOutput->getType());

	// Make sure the user actually registered the node
	assert(node == nullptr || getCachedInstance(binOp, context) != nullptr);

	node->setIrContext(context);
	node->setIrStructure(binOp);

	return node;
}

piranha::Node *piranha::LanguageRules::getCachedInstance(IrParserStructure *ir, IrContextTree *context) {
	// TODO: this lookup could be made faster by having the lookup table be a tree
	// with the first level being a lookup by node and the second by context

	int nodeCount = getNodeCount();
	for (int i = 0; i < nodeCount; i++) {
		if (m_nodes[i]->getIrStructure() == ir) {
			if (m_nodes[i]->getContext()->isEqual(context)) {
				return m_nodes[i];
			}
		}
	}

	return nullptr;
}

piranha::Node *piranha::LanguageRules::generateOperator(IrBinaryOperator::OPERATOR op, const NodeType *left, const NodeType *right) {
	OperatorRule *rule = m_operatorRules.lookup({ op, left, right });
	if (rule == nullptr) return nullptr;

	Node *newNode = rule->buildNode();
	m_nodes.push_back(newNode);

	return newNode;
}

piranha::Node *piranha::LanguageRules::generateBuiltinType(const std::string &typeName) {
	BuiltinRule *rule = m_builtinRules.lookup(typeName);
	if (rule == nullptr) return nullptr;

	Node *newNode = rule->buildNode();
	m_nodes.push_back(newNode);

	return newNode;
}

piranha::Node *piranha::LanguageRules::generateConversion(const NodeTypeConversion &conversion) {
	ConversionRule *rule = m_conversionRules.lookup(conversion);
	if (rule == nullptr) return nullptr;

	Node *newNode = rule->buildNode();
	m_nodes.push_back(newNode);

	return newNode;
}

piranha::Node *piranha::LanguageRules::generateCustomType() {
	Node *newNode = allocateNode<CustomNode>();
	return newNode;
}
