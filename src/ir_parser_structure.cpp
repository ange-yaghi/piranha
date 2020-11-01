#include "../include/ir_parser_structure.h"

#include "../include/ir_node.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_context_tree.h"
#include "../include/node_program.h"
#include "../include/node.h"
#include "../include/language_rules.h"
#include "../include/compilation_error.h"
#include "../include/memory_tracker.h"

piranha::IrParserStructure::IrReferenceInfo::IrReferenceInfo() {
    newContext = nullptr;
    err = nullptr;

    failed = false;
    infiniteLoop = -1;
    reachedDeadEnd = false;
    touchedMainContext = false;

    fixedType = nullptr;
    fixedTypeContext = nullptr;
    staticType = true;
}

piranha::IrParserStructure::IrReferenceInfo::~IrReferenceInfo() {
    /* void */
}

void piranha::IrParserStructure::IrReferenceInfo::reset() {
    newContext = nullptr;
    err = nullptr;

    failed = false;
    infiniteLoop = -1;
    reachedDeadEnd = false;
    touchedMainContext = false;

    fixedType = nullptr;
    fixedTypeContext = nullptr;
    staticType = true;
}

piranha::IrParserStructure::IrReferenceQuery::IrReferenceQuery() {
    inputContext = nullptr;
    recordErrors = false;
    recordInfiniteLoops = false;
}

piranha::IrParserStructure::IrReferenceQuery::~IrReferenceQuery() {
    /* void */
}

void piranha::IrParserStructure::IrReferenceChain::
    addLink(IrParserStructure *structure, IrContextTree *context) 
{
    list.push_back({ structure, context });
}

int piranha::IrParserStructure::IrReferenceChain::
    searchLink(IrParserStructure *structure, IrContextTree *context) 
{
    const int links = (int)list.size();
    for (int i = 0; i < links; i++) {
        const Link &link = list[i];
        if (link.structure == structure && link.context->isEqual(context)) {
            return i;
        }
    }

    return -1;
}

piranha::IrParserStructure::IrParserStructure() {
    m_scopeParent = nullptr;
    m_logicalParent = nullptr;
    m_checkReferences = true;
    m_parentUnit = nullptr;

    m_definitionsResolved = false;
    m_expanded = false;
    m_validated = false;
    m_typesChecked = false;
    m_instantiationChecked = false;

    m_visibility = IrVisibility::Default;
    m_defaultVisibility = IrVisibility::Private;

    m_rules = nullptr;
}

piranha::IrParserStructure::~IrParserStructure() {
    /* void */
}

void piranha::IrParserStructure::setRules(const LanguageRules *rules) {
    // Set component rules
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->setRules(rules);
    }

    m_rules = rules;
}

void piranha::IrParserStructure::registerToken(const IrTokenInfo *tokenInfo) {
    if (tokenInfo != nullptr) m_summaryToken.combine(tokenInfo);
}

void piranha::IrParserStructure::registerComponent(IrParserStructure *child) {
    if (child != nullptr) {
        m_summaryToken.combine(child->getSummaryToken());
        child->setScopeParent(this);
        child->setLogicalParent(this);

        m_components.push_back(child);
    }
}

piranha::IrParserStructure *piranha::IrParserStructure::resolveName(const std::string &name) const {
    IrParserStructure *local = resolveLocalName(name);
    if (local != nullptr) return local;
    
    if (m_scopeParent != nullptr) {
        return m_scopeParent->resolveName(name);
    }

    return nullptr;
}

piranha::IrParserStructure *piranha::IrParserStructure::getImmediateReference(
    const IrReferenceQuery &query, IrReferenceInfo *output) 
{
    return nullptr;
}

piranha::IrParserStructure *piranha::IrParserStructure::getReference(
    const IrReferenceQuery &query, IrReferenceInfo *output) 
{
    IrReferenceChain chain;
    chain.addLink(this, query.inputContext);

    IrReferenceInfo info;
    IrParserStructure *reference = getReference(query, &info, &chain);

    if (output != nullptr) *output = info;

    if (query.recordErrors && info.infiniteLoop >= 0) {
        const IrReferenceChain::Link &link = chain.list[info.infiniteLoop];

        if (!link.structure->isInfiniteLoop(link.context)) {
            if (output != nullptr && output->touchedMainContext || IR_EMPTY_CONTEXT()) {
                IR_ERR_OUT(TRACK(new CompilationError(*link.structure->getSummaryToken(),
                    ErrorCode::CircularReference, link.context)));
            }
        }

        for (int i = info.infiniteLoop; i < (int)chain.list.size(); i++) {
            chain.list[i].structure->setInfiniteLoop(chain.list[i].context);
        }
    }

    return reference;
}

piranha::IrParserStructure *piranha::IrParserStructure::getReference(
    const IrReferenceQuery &query, IrReferenceInfo *output, IrReferenceChain *chain)
{
    IR_RESET(query);

    IrReferenceQuery immediateQuery = query;
    IrReferenceInfo immediateInfo;
    IrParserStructure *immediateReference =
        getImmediateReference(immediateQuery, &immediateInfo);

    if (immediateInfo.failed) {
        IR_FAIL();
        IR_ERR_OUT(immediateInfo.err);
        return nullptr;
    }

    if (immediateInfo.reachedDeadEnd) {
        IR_DEAD_END();
        if (immediateInfo.isFixedType()) {
            IR_INFO_OUT(fixedType, immediateInfo.fixedType);
            IR_INFO_OUT(fixedTypeContext, immediateInfo.fixedTypeContext);
        }
        return nullptr;
    }

    if (immediateReference != nullptr) {
        // Check for an infinite loop
        const int infiniteLoop = chain->searchLink(immediateReference, immediateInfo.newContext);
        if (infiniteLoop >= 0) {
            IR_FAIL();
            IR_INFO_OUT(infiniteLoop, infiniteLoop);
            IR_INFO_OUT(touchedMainContext, immediateInfo.touchedMainContext);
            return nullptr;
        }
        else {
            chain->addLink(immediateReference, immediateInfo.newContext);
        }

        IrReferenceInfo nestedInfo;
        IrReferenceQuery nestedQuery;
        nestedQuery.inputContext = immediateInfo.newContext;

        // Error checking is not done on any parent nodes because it's assumed that errors have
        // already been checked/reported
        nestedQuery.recordErrors = false;
        nestedQuery.recordInfiniteLoops = query.recordErrors || query.recordInfiniteLoops;

        IrParserStructure *fullReference =
            immediateReference->getReference(nestedQuery, &nestedInfo, chain);

        IR_INFO_OUT(touchedMainContext,
            nestedInfo.touchedMainContext ||
            immediateInfo.touchedMainContext
        );

        if (nestedInfo.failed) {
            IR_FAIL();
            IR_ERR_OUT(nestedInfo.err);
            IR_INFO_OUT(infiniteLoop, nestedInfo.infiniteLoop);
            return nullptr;
        }

        // Immediate takes precedence
        // NOTE - this has to be done here because even when reaching a dead end
        // fixed type information can still be used
        if (immediateInfo.isFixedType()) {
            IR_INFO_OUT(staticType, immediateInfo.staticType);
            IR_INFO_OUT(fixedType, immediateInfo.fixedType);
            IR_INFO_OUT(fixedTypeContext, immediateInfo.newContext);
        }
        else if (nestedInfo.isFixedType()) {
            IR_INFO_OUT(staticType, immediateInfo.staticType && nestedInfo.staticType);
            IR_INFO_OUT(fixedType, nestedInfo.fixedType);
            IR_INFO_OUT(fixedTypeContext, nestedInfo.newContext);
        }

        if (nestedInfo.reachedDeadEnd) {
            IR_DEAD_END();
            return nullptr;
        }

        IR_INFO_OUT(newContext, nestedInfo.newContext);

        return fullReference;
    }
    else return this;
}

void piranha::IrParserStructure::resolveDefinitions() {
    if (m_definitionsResolved) return;
    else m_definitionsResolved = true;

    // Resolve components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->resolveDefinitions();
    }

    _resolveDefinitions();
}

void piranha::IrParserStructure::checkCircularDefinitions() {
    /* void */
}

void piranha::IrParserStructure::checkCircularDefinitions(IrContextTree *context, IrNodeDefinition *root) {
    // Check components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        if (m_components[i]->getCheckReferences()) {
            m_components[i]->checkCircularDefinitions(context, root);
        }
    }

    _checkCircularDefinitions(context, root);
}

void piranha::IrParserStructure::expand() {
    if (m_expanded) return;
    else m_expanded = true;

    // Check components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->expand();
    }

    _expand();
}

void piranha::IrParserStructure::expand(IrContextTree *context) {
    if (m_expansions.lookup(context) != nullptr) return;
    else *m_expansions.newValue(context) = nullptr;

    // Expand components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        if (m_components[i]->getCheckReferences()) {
            m_components[i]->expand(context);
        }
    }

    // Perform the actual expansion
    _expand(context);
}

void piranha::IrParserStructure::expandChain(IrContextTree *context) {
    IrReferenceChain chain;
    expandChain(context, &chain);
}

void piranha::IrParserStructure::expandChain(IrContextTree *context, IrReferenceChain *chain) {
    if (chain->searchLink(this, context) >= 0) return;
    else chain->addLink(this, context);

    expand(context);

    // Expand reference
    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;
    IrParserStructure *immediateReference = getImmediateReference(query, &info);

    if (immediateReference != nullptr) {
        immediateReference->expandChain(info.newContext, chain);
    }
}

void piranha::IrParserStructure::checkReferences(IrContextTree *inputContext) {
    // Check components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkReferences(inputContext);
    }

    if (m_checkReferences) {
        IrReferenceQuery query;
        query.inputContext = inputContext;
        query.recordErrors = true;
        IrReferenceInfo info;

        IrParserStructure *reference = getReference(query, &info);

        if (info.err != nullptr) {
            getParentUnit()->addCompilationError(info.err);
        }
    }
}

void piranha::IrParserStructure::checkTypes(IrContextTree *inputContext) {
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkTypes(inputContext);
    }

    _checkTypes(inputContext);
}

void piranha::IrParserStructure::checkInstantiation() {
    if (m_instantiationChecked) return;
    m_instantiationChecked = true;

    // Check components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkInstantiation();
    }

    _checkInstantiation();
}

void piranha::IrParserStructure::checkTypes() {
    if (m_typesChecked) return;
    m_typesChecked = true;

    // Check components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkTypes();
    }

    _checkTypes();
}

void piranha::IrParserStructure::validate() {
    if (m_validated) return;
    m_validated = true;

    // Validate components
    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->validate();
    }

    _validate();  
}

void piranha::IrParserStructure::_validate() {
    /* void */
}

void piranha::IrParserStructure::_checkTypes() {
    /* void */
}

void piranha::IrParserStructure::_checkTypes(IrContextTree *context) {
    /* void */
}

void piranha::IrParserStructure::_checkInstantiation() {
    /* void */
}

void piranha::IrParserStructure::writeReferencesToFile(
    std::ofstream &file, IrContextTree *context, int tabLevel) 
{
    for (int i = 0; i < tabLevel; i++) {
        file << " ";
    }

    if (m_summaryToken.lineStart != m_summaryToken.lineEnd) {
        file << "L[" << m_summaryToken.lineStart << "-" << m_summaryToken.lineEnd << "]";
    }
    else {
        file << "L[" << m_summaryToken.lineStart << "]";
    }

    file << "[" << m_summaryToken.colStart << "-" << m_summaryToken.colEnd << "]";

    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;

    IrNode *asNode = getAsNode();
    IrParserStructure *immediateReference = getImmediateReference(query, &info);

    if (info.failed) {
        file << " => "; 
        file << "ERROR" << std::endl;
        return;
    }
    else if (immediateReference == nullptr) {
        file << std::endl;
        return;
    }
    else {
        file << " => ";
        immediateReference->writeReferencesToFile(file, info.newContext);
    }
}

void piranha::IrParserStructure::_checkCircularDefinitions(IrContextTree *context, IrNodeDefinition *root) {
    /* void */
}

void piranha::IrParserStructure::_resolveDefinitions() {
    /* void */
}

void piranha::IrParserStructure::_expand() {
    /* void */
}

void piranha::IrParserStructure::_expand(IrContextTree *inputContext) {
    /* void */
}

piranha::IrParserStructure *piranha::IrParserStructure::resolveLocalName(
    const std::string &name) const 
{
    return nullptr;
}

bool piranha::IrParserStructure::allowsExternalAccess() const {
    IrVisibility visibility = (m_visibility == IrVisibility::Default) 
        ? m_defaultVisibility 
        : m_visibility;

    return visibility == IrVisibility::Public;
}

piranha::IrCompilationUnit *piranha::IrParserStructure::getParentUnit() const {
    if (m_parentUnit == nullptr) return m_logicalParent->getParentUnit();
    else return m_parentUnit;
}

bool piranha::IrParserStructure::isInfiniteLoop(IrContextTree *context) {
    const int infiniteLoops = (int)m_infiniteLoops.size();
    for (int i = 0; i < infiniteLoops; i++) {
        if (m_infiniteLoops[i]->isEqual(context)) {
            return true;
        }
    }

    return false;
}

void piranha::IrParserStructure::setInfiniteLoop(IrContextTree *context) {
    if (isInfiniteLoop(context)) return;

    m_infiniteLoops.push_back(context);
}

void piranha::IrParserStructure::free() {
    for (IrParserStructure *component : m_components) {
        component->free();

        delete FTRACK(component);
    }

    for (IrContextTree *tree : m_trees) {
        tree->free();

        delete FTRACK(tree);
    }
    
    const int expansionCount = m_expansions.getEntryCount();
    for (int i = 0; i < expansionCount; ++i) {
        IrNode *expansion = *m_expansions.get(i);

        if (expansion != nullptr) {
            expansion->free();
            delete FTRACK(expansion);
        }
    }

    m_expansions.destroy();
}

piranha::NodeOutput *piranha::IrParserStructure::generateNodeOutput(
    IrContextTree *context, NodeProgram *program, NodeContainer *container)
{
    Node *node = program->getCachedInstance(this, context);

    if (node == nullptr) node = generateNode(context, program, container);
    if (node != nullptr) return node->generateAliasOutput();
    else return _generateNodeOutput(context, program, container);
}

piranha::Node *piranha::IrParserStructure::generateNode(
    IrContextTree *context, NodeProgram *program, NodeContainer *container)
{
    Node *node = program->getCachedInstance(this, context);

    if (node != nullptr) return node->generateAliasNode();
    else {
        node = _generateNode(context, program, container);
        if (node != nullptr) {
            return node->generateAliasNode();
        }
        else return nullptr;
    }
}

piranha::NodeOutput *piranha::IrParserStructure::_generateNodeOutput(
    IrContextTree *context, NodeProgram *program, NodeContainer *container)
{
    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;
    IrParserStructure *immediateReference = getImmediateReference(query, &info);

    if (immediateReference == nullptr) return nullptr;
    else return immediateReference->generateNodeOutput(info.newContext, program, container);
}

piranha::Node *piranha::IrParserStructure::_generateNode(
    IrContextTree *context, NodeProgram *program, NodeContainer *container)
{
    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;
    IrParserStructure *immediateReference = getImmediateReference(query, &info);

    if (immediateReference == nullptr) return nullptr;
    else return immediateReference->generateNode(info.newContext, program, container);
}
