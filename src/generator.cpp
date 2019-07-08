#include "generator.h"

#include "node_builder.h"
#include "node.h"
#include "ir_node.h"
#include "ir_context_tree.h"
#include "ir_node_definition.h"

piranha::IrGenerator::IrGenerator() {
	/* void */
}

piranha::IrGenerator::~IrGenerator() {
	/* void */
}

piranha::Node *piranha::IrGenerator::generateNode(IrNode *node, IrContextTree *context) {
	Node *cachedNode = getCachedInstance(node, context);
	if (cachedNode != nullptr) return cachedNode;
	else {
		IrNodeDefinition *definition = node->getDefinition();
		if (definition == nullptr) return nullptr;

		if (definition->isBuiltin()) {
			return generateBuiltinType(
				definition->getBuiltinName()
			);
		}
		else {
			return generateCustomType(node, context);
		}
	}
}

piranha::Node *piranha::IrGenerator::getCachedInstance(IrParserStructure *ir, IrContextTree *context) {
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

piranha::Node *piranha::IrGenerator::generateBuiltinType(const std::string &typeName) {
	int nodeTypeCount = getNodeTypeCount();
	for (int i = 0; i < nodeTypeCount; i++) {
		if (m_nodeBuilders[i]->checkName(typeName)) {
			return m_nodeBuilders[i]->buildNode();
		}
	}

	return nullptr;
}

piranha::Node *piranha::IrGenerator::generateCustomType(IrNode *node, IrContextTree *context) {
	return nullptr;
}

void piranha::IrGenerator::registerNodeBuilder(NodeBuilder *builder) {
	m_nodeBuilders.push_back(builder);
}
