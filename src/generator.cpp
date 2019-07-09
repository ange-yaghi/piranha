#include "generator.h"

#include "node_builder.h"
#include "node.h"
#include "custom_node.h"
#include "ir_node.h"
#include "ir_context_tree.h"
#include "ir_node_definition.h"
#include "ir_value_constant.h"

#include <assert.h>

piranha::Generator::Generator() {
	/* void */
}

piranha::Generator::~Generator() {
	/* void */
}

piranha::Node *piranha::Generator::generateLiteral(IrValue *ir, IrContextTree *context) {
	Node *cachedNode = getCachedInstance(ir, context);
	if (cachedNode != nullptr) return cachedNode;
	else {
		Node *newNode = generateLiteral(ir->getType());
		newNode->setIrContext(context);
		newNode->setIrStructure(ir);

		return newNode;
	}
}

piranha::Node *piranha::Generator::generateNode(IrNode *node, IrContextTree *context) {
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

piranha::Node *piranha::Generator::generateOperatorNode(IrBinaryOperator *binOp, IrContextTree *context) {
	IrValue *left = binOp->getLeft();
	IrValue *right = binOp->getRight();

	NodeOutput *leftNodeOutput = left->generateNodeOutput(context, m_nodeProgram);
	NodeOutput *rightNodeOutput = right->generateNodeOutput(context, m_nodeProgram);
	
	Node *node = generateOperator(binOp->getOperator(), leftNodeOutput, rightNodeOutput);

	// Make sure the user actually registered the node
	assert(node == nullptr || getCachedInstance(binOp, context) != nullptr);

	node->setIrContext(context);
	node->setIrStructure(binOp);

	return node;
}

piranha::Node *piranha::Generator::getCachedInstance(IrParserStructure *ir, IrContextTree *context) {
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

piranha::Node *piranha::Generator::generateBuiltinType(const std::string &typeName) {
	int nodeTypeCount = getNodeTypeCount();
	for (int i = 0; i < nodeTypeCount; i++) {
		if (m_nodeBuilders[i]->checkKey(typeName)) {
			Node *newNode = m_nodeBuilders[i]->buildNode();
			m_nodes.push_back(newNode);

			return newNode;
		}
	}

	return nullptr;
}

piranha::Node *piranha::Generator::generateLiteral(IrValue::VALUE_TYPE valueType) {
	int literalTypeCount = getLiteralTypeCount();
	for (int i = 0; i < literalTypeCount; i++) {
		if (m_valueBuilders[i]->checkKey(valueType)) {
			Node *newNode = m_valueBuilders[i]->buildNode();
			m_nodes.push_back(newNode);

			return newNode;
		}
	}

	return nullptr;
}

piranha::Node *piranha::Generator::generateCustomType() {
	Node *newNode = allocateNode<CustomNode>();
	return newNode;
}

void piranha::Generator::registerNodeBuilder(BuiltinBuilder *builder) {
	m_nodeBuilders.push_back(builder);
}

void piranha::Generator::registerLiteralBuilder(LiteralBuilder *builder) {
	m_valueBuilders.push_back(builder);
}
