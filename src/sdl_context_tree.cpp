#include <sdl_context_tree.h>

#include <sdl_node.h>
#include <sdl_attribute_definition.h>
#include <sdl_attribute_list.h>
#include <sdl_attribute.h>

piranha::SdlContextTree::SdlContextTree() {
	m_context = nullptr;
	m_parent = nullptr;
}

piranha::SdlContextTree::SdlContextTree(SdlNode *context, bool mainContext) {
	m_context = context;
	m_mainContext = mainContext;
}

piranha::SdlContextTree::~SdlContextTree() {
	/* void */
}

piranha::SdlContextTree *piranha::SdlContextTree::getRoot() {
	if (m_parent != nullptr) return m_parent->getRoot();
	else return this;
}

piranha::SdlContextTree *piranha::SdlContextTree::search(const SdlContextTree *reference) const {
	return nullptr;
}

piranha::SdlContextTree *piranha::SdlContextTree::newChild(SdlNode *context, bool mainContext) {
	SdlContextTree *newChild = new SdlContextTree(context, mainContext);
	newChild->setParent(this);

	m_children.push_back(newChild);

	return newChild;
}

piranha::SdlContextTree *piranha::SdlContextTree::findContext(SdlParserStructure *context) {
	if (m_context == context) return this;
	else if (m_parent != nullptr) return m_parent->findContext(context);
	else return nullptr;
}

piranha::SdlParserStructure *piranha::SdlContextTree::resolveDefinition(SdlAttributeDefinition *definition) {
	if (m_context != nullptr) {
		SdlAttributeList *attributes = m_context->getAttributes();
		if (attributes != nullptr) {
			return attributes->getAttribute(definition);
		}
	}

	return nullptr;
}

bool piranha::SdlContextTree::isEqual(const SdlContextTree *ref) const {
	const SdlContextTree *currentRefNode = ref;
	const SdlContextTree *currentNode = this;

	while (true) {
		if (currentNode->getContext() != currentRefNode->getContext()) return false;
		if (currentNode->getContext() == nullptr || currentRefNode->getContext() == nullptr) break;

		currentRefNode = currentRefNode->getParent();
		currentNode = currentNode->getParent();
	}

	return (currentNode->getContext() == currentRefNode->getContext());
}
