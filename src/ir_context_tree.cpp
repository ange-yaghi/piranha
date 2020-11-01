#include "../include/ir_context_tree.h"

#include "../include/ir_node.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_attribute.h"
#include "../include/memory_tracker.h"

piranha::IrContextTree::IrContextTree() {
    m_context = nullptr;
    m_parent = nullptr;
    m_mainContext = false;
}

piranha::IrContextTree::IrContextTree(IrNode *context, bool mainContext) {
    m_context = context;
    m_mainContext = mainContext;
    m_parent = nullptr;
}

piranha::IrContextTree::~IrContextTree() {
    /* void */
}

piranha::IrContextTree *piranha::IrContextTree::getRoot() {
    if (m_parent != nullptr) return m_parent->getRoot();
    else return this;
}

piranha::IrContextTree *piranha::IrContextTree::getMain() {
    return getRoot()->_getMain();
}

void piranha::IrContextTree::free() {
    for (IrContextTree *tree : m_children) {
        tree->free();

        delete FTRACK(tree);
    }

    m_children.clear();
}

piranha::IrContextTree *piranha::IrContextTree::_getMain() {
    if (isMainContext()) return this;

    const int childCount = getChildCount();
    for (int i = 0; i < childCount; i++) {
        IrContextTree *mainRoot;
        mainRoot = m_children[i]->_getMain();

        if (mainRoot != nullptr) return mainRoot;
    }

    return nullptr;
}

piranha::IrContextTree *piranha::IrContextTree::newChild(IrNode *context, bool mainContext) {
    IrContextTree *newChild = TRACK(new IrContextTree(context, mainContext));
    newChild->setParent(this);

    m_children.push_back(newChild);

    return newChild;
}

bool piranha::IrContextTree::hasParent(const IrContextTree *context) const {
    if (m_parent != nullptr) {
        if (m_parent->isEqual(context)) return true;
        else return m_parent->hasParent(context);
    }
    else return false;
}

piranha::IrContextTree *piranha::IrContextTree::findContext(IrNode *context) {
    if (m_context == context) return this;
    else if (m_parent != nullptr) return m_parent->findContext(context);
    else return nullptr;
}

piranha::IrParserStructure *piranha::IrContextTree::resolveDefinition(
    IrAttributeDefinition *definition) 
{
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
        if (currentNode->isEmpty() || currentRefNode->isEmpty()) break;

        currentRefNode = currentRefNode->getParent();
        currentNode = currentNode->getParent();

        if (currentNode == nullptr || currentRefNode == nullptr) {
            return currentNode == currentRefNode;
        }
    }

    return (currentNode->getContext() == currentRefNode->getContext());
}

bool piranha::IrContextTree::isOutside(const IrContextTree *ref) const {
    const IrContextTree *currentRefNode = this;

    while (currentRefNode != nullptr) {
        if (ref->isEqual(currentRefNode)) return false;
        else if (ref->hasParent(currentRefNode)) return true;
        else currentRefNode = currentRefNode->getParent();
    }

    return false;
}
