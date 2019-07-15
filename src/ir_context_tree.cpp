#include "../include/ir_context_tree.h"

#include "../include/ir_node.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_attribute.h"

piranha::IrContextTree::IrContextTree() {
	m_context = nullptr;
	m_parent = nullptr;
}

piranha::IrContextTree::IrContextTree(IrNode *context, bool mainContext) {
	m_context = context;
	m_mainContext = mainContext;
}

piranha::IrContextTree::~IrContextTree() {
	/* void */
}

piranha::IrContextTree *piranha::IrContextTree::getRoot() {
	if (m_parent != nullptr) return m_parent->getRoot();
	else return this;
}

piranha::IrContextTree *piranha::IrContextTree::search(const IrContextTree *reference) const {
	return nullptr;
}

piranha::IrContextTree *piranha::IrContextTree::newChild(IrNode *context, bool mainContext) {
	IrContextTree *newChild = new IrContextTree(context, mainContext);
	newChild->setParent(this);

	m_children.push_back(newChild);

	return newChild;
}

piranha::IrContextTree *piranha::IrContextTree::findContext(IrParserStructure *context) {
	if (m_context == context) return this;
	else if (m_parent != nullptr) return m_parent->findContext(context);
	else return nullptr;
}

piranha::IrParserStructure *piranha::IrContextTree::resolveDefinition(IrAttributeDefinition *definition) {
	if (m_context != nullptr) {
		IrAttributeList *attributes = m_context->getAttributes();
		if (attributes != nullptr) {
			return attributes->getAttribute(definition);
		}
	}

	return nullptr;
}

bool piranha::IrContextTree::isEqual(const IrContextTree *ref) const {
	const IrContextTree *currentRefNode = ref;
	const IrContextTree *currentNode = this;

	while (true) {
		if (currentNode->getContext() != currentRefNode->getContext()) return false;
		if (currentNode->getContext() == nullptr || currentRefNode->getContext() == nullptr) break;

		currentRefNode = currentRefNode->getParent();
		currentNode = currentNode->getParent();

		if (currentNode == nullptr || currentRefNode == nullptr) return currentNode == currentRefNode;
	}

	return (currentNode->getContext() == currentRefNode->getContext());
}
