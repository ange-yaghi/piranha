#include "../include/ir_compilation_unit.h"

#include "../include/scanner.h"
#include "../include/ir_node.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_attribute.h"
#include "../include/error_list.h"
#include "../include/node_program.h"
#include "../include/language_rules.h"
#include "../include/memory_tracker.h"

#include <cctype>
#include <fstream>
#include <cassert>
#include <sstream>

piranha::IrCompilationUnit::~IrCompilationUnit() {
    /* void */
}

void piranha::IrCompilationUnit::build(NodeProgram *program) {
    IrContextTree *context = TRACK(new IrContextTree(nullptr));

    program->setRootUnit(this);
    program->addContainer(context, program->getTopLevelContainer());

    const int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        m_nodes[i]->generateNode(context, program, program->getTopLevelContainer());
    }

    program->setRootContext(context);
}

piranha::IrCompilationUnit::ParseResult piranha::IrCompilationUnit::parseFile(
    const Path &filename, IrCompilationUnit *topLevel) 
{
    m_path = filename;
    
    std::ifstream inFile(filename.toString());
    if (!inFile.good()) {
        return ParseResult::IoError;
    }

    return parseHelper(inFile);
}

piranha::IrCompilationUnit::ParseResult piranha::IrCompilationUnit::parse(
    const char *sdl, IrCompilationUnit *topLevel) 
{
    std::istringstream sdlStream(sdl);
    return parse(sdlStream);
}

piranha::IrCompilationUnit::ParseResult piranha::IrCompilationUnit::parse(
    std::istream &stream, IrCompilationUnit *topLevel)
{
    if (!stream.good() && stream.eof()) {
        return ParseResult::IoError;
    }

    return parseHelper(stream);
}

void piranha::IrCompilationUnit::_checkInstantiation() {
    IrContextTree *mainContext = TRACK(new IrContextTree(nullptr, true));

    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkReferences(mainContext);
    }

    addTree(mainContext);
}

void piranha::IrCompilationUnit::_checkTypes() {
    IrContextTree *mainContext = TRACK(new IrContextTree(nullptr, true));

    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->checkTypes(mainContext);
    }

    addTree(mainContext);
}

void piranha::IrCompilationUnit::resolveAll() {
    if (m_unitResolved) return;
    else m_unitResolved = true;

    const int dependencyCount = getDependencyCount();
    for (int i = 0; i < dependencyCount; i++) {
        getDependency(i)->resolveAll();
    }

    resolveDefinitions();

    // Check for circular definitions
    const int definitionCount = getNodeDefinitionCount();
    for (int i = 0; i < definitionCount; i++) {
        getNodeDefinition(i)->checkCircularDefinitions();
    }

    expand();
    checkInstantiation();
    checkTypes();
}

void piranha::IrCompilationUnit::validateAll() {
    if (m_unitValidated) return;
    else m_unitValidated = true;

    const int dependencyCount = getDependencyCount();
    for (int i = 0; i < dependencyCount; i++) {
        getDependency(i)->validateAll();
    }

    validate();
}

void piranha::IrCompilationUnit::registerStructure(IrParserStructure *structure) {
    m_allStructures.push_back(structure);
}

void piranha::IrCompilationUnit::free() {
    IrParserStructure::free();

    delete FTRACK(m_parser);
    delete FTRACK(m_scanner);
}

void piranha::IrCompilationUnit::_expand() {
    IrContextTree *mainContext = TRACK(new IrContextTree(nullptr, true));

    const int componentCount = getComponentCount();
    for (int i = 0; i < componentCount; i++) {
        m_components[i]->expand(mainContext);
    }

    addTree(mainContext);
}

piranha::IrCompilationUnit::ParseResult piranha::IrCompilationUnit::parseHelper(
    std::istream &stream, IrCompilationUnit *topLevel) 
{
    if (m_scanner != nullptr) delete FTRACK(m_scanner);
    try {
        m_scanner = TRACK(new piranha::Scanner(&stream));
    }
    catch (std::bad_alloc) {
        return ParseResult::Fail;
    }

    if (m_parser != nullptr) delete FTRACK(m_parser);
    try {
        m_parser = TRACK(new piranha::Parser(*m_scanner, *this));
    }
    catch (std::bad_alloc) {
        return ParseResult::Fail;
    }

    if (m_parser->parse() != 0) {
        return ParseResult::Fail;
    }
    else return ParseResult::Success;
}

piranha::IrNodeDefinition *piranha::IrCompilationUnit::resolveNodeDefinition(
    const std::string &name, int *count, const std::string &libraryName, bool external) 
{
    *count = 0;
    piranha::IrNodeDefinition *firstDefinition = nullptr;
    const std::string typeName = name;

    // First search local node definitions if a library is not specified
    if (libraryName.empty()) {
        int localCount = 0;
        IrNodeDefinition *localDefinition = 
            resolveLocalNodeDefinition(typeName, &localCount, external);
        (*count) += localCount;

        if (localDefinition != nullptr) return localDefinition;
    }

    // Search dependencies
    const int dependencyCount = getImportStatementCount();
    for (int i = 0; i < dependencyCount; i++) {
        int secondaryCount = 0;
        IrImportStatement *importStatement = getImportStatement(i);

        // Skip the import statement if it already failed
        if (importStatement->getUnit() == nullptr) continue;

        const bool libraryNameMatches = importStatement->hasShortName()
            ? libraryName == importStatement->getShortName()
            : false;

        if (libraryName.empty() || libraryNameMatches) {
            // Skip the import statement if it can't be accessed
            if (external && !importStatement->allowsExternalAccess()) continue;

            // The external access flag must be set to true since the libraries are being accessed
            // externally                                                                        ----
            IrNodeDefinition *definition =                                                       ////
                importStatement->getUnit()->resolveNodeDefinition(typeName, &secondaryCount, "", true);
            if (definition != nullptr) {
                (*count) += secondaryCount;

                // Make sure to not overwrite the result definition
                // The first definition to be found must be returned
                if (firstDefinition == nullptr) {
                    firstDefinition = definition;
                }
            }
        }
    }

    return firstDefinition;
}

piranha::IrNodeDefinition *piranha::IrCompilationUnit::resolveLocalBuiltinNodeDefinition(
    const std::string &builtinName, int *count, bool external) 
{
    *count = 0;
    piranha::IrNodeDefinition *firstDefinition = nullptr;
    const std::string typeName = builtinName;

    const int localNodeDefinitions = (int)m_nodeDefinitions.size();
    for (int i = 0; i < localNodeDefinitions; i++) {
        IrNodeDefinition *definition = m_nodeDefinitions[i];
        if (external && !definition->allowsExternalAccess()) continue;
        if (!definition->isBuiltin()) continue;

        if (definition->getBuiltinName() == typeName) {
            (*count)++;
            if (firstDefinition == nullptr) {
                firstDefinition = definition;
            }
        }
    }

    return firstDefinition;
}

piranha::IrNodeDefinition *piranha::IrCompilationUnit::resolveBuiltinNodeDefinition(
    const std::string &builtinName, int *count, bool external) 
{
    piranha::IrNodeDefinition *firstDefinition = nullptr;
    const std::string typeName = builtinName;

    // First search local node definitions if a library is not specified
    int localCount = 0;
    IrNodeDefinition *localDefinition = 
        resolveLocalBuiltinNodeDefinition(typeName, &localCount, external);
    (*count) = localCount;

    if (localDefinition != nullptr) return localDefinition;

    // Search dependencies
    const int dependencyCount = getImportStatementCount();
    for (int i = 0; i < dependencyCount; i++) {
        IrImportStatement *importStatement = getImportStatement(i);

        // Skip the import statement if it already failed
        if (importStatement->getUnit() == nullptr) continue;

        // Skip if the import statement cannot be accessed
        if (external && !importStatement->allowsExternalAccess()) continue;

        int secondaryCount = 0;

        // The external access flag must be set to true since the libraries are being accessed
        // externally                                                                           ----
        IrNodeDefinition *definition =                                                          ////
            importStatement->getUnit()->resolveBuiltinNodeDefinition(typeName, &secondaryCount, true);
        if (definition != nullptr) {
            (*count) += secondaryCount;
            if (firstDefinition == nullptr) {
                firstDefinition = definition;
            }
        }
    }

    return firstDefinition;
}

piranha::IrNodeDefinition *piranha::IrCompilationUnit::resolveLocalNodeDefinition(
    const std::string &name, int *count, bool external) 
{
    *count = 0;
    piranha::IrNodeDefinition *definition = nullptr;
    const std::string typeName = name;

    const int localNodeDefinitions = (int)m_nodeDefinitions.size();
    for (int i = 0; i < localNodeDefinitions; i++) {
        IrNodeDefinition *def = m_nodeDefinitions[i];
        if (external && !def->allowsExternalAccess()) continue;

        const std::string defName = m_nodeDefinitions[i]->getName();
        if (defName == typeName) {
            ++(*count);
            if (definition == nullptr) {
                definition = m_nodeDefinitions[i];
            }
        }
    }

    return definition;
}

void piranha::IrCompilationUnit::_validate() {
    const int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        IrNode *node = m_nodes[i];
        const int count = countSymbolIncidence(node->getName());

        if (count > 1) {
            this->addCompilationError(
                TRACK(new CompilationError(node->getNameToken(),
                ErrorCode::SymbolUsedMultipleTimes)));
        }
    }

    const int definitionCount = getNodeDefinitionCount();
    for (int i = 0; i < definitionCount; i++) {
        IrNodeDefinition *def = m_nodeDefinitions[i];
        int count = 0;
        resolveLocalNodeDefinition(def->getName(), &count);

        if (count > 1) {
            this->addCompilationError(
                TRACK(new CompilationError(*def->getNameToken(),
                ErrorCode::DuplicateNodeDefinition)));
        }
    }
}

void piranha::IrCompilationUnit::addNode(IrNode *node) {
    if (node != nullptr) {
        m_nodes.push_back(node);
        registerComponent(node);

        node->setParentUnit(this);
    }
}

piranha::IrNode *piranha::IrCompilationUnit::getNode(int index) const {
    return m_nodes[index];
}

int piranha::IrCompilationUnit::getNodeCount() const {
    return (int)m_nodes.size();
}

void piranha::IrCompilationUnit::addImportStatement(IrImportStatement *statement) {
    if (statement != nullptr) {
        m_importStatements.push_back(statement);
        registerComponent(statement);

        statement->setParentUnit(this);
    }
}

piranha::IrImportStatement *piranha::IrCompilationUnit::getImportStatement(int index) const {
    return m_importStatements[index];
}

int piranha::IrCompilationUnit::getImportStatementCount() const {
    return (int)m_importStatements.size();
}

void piranha::IrCompilationUnit::addNodeDefinition(IrNodeDefinition *nodeDefinition) {
    if (nodeDefinition != nullptr) {
        m_nodeDefinitions.push_back(nodeDefinition);
        registerComponent(nodeDefinition);

        nodeDefinition->setParentUnit(this);
        nodeDefinition->setScopeParent(this);
    }
}

piranha::IrNodeDefinition *piranha::IrCompilationUnit::getNodeDefinition(int index) const {
    return m_nodeDefinitions[index];
}

int piranha::IrCompilationUnit::getNodeDefinitionCount() const {
    return (int)m_nodeDefinitions.size();
}

piranha::IrParserStructure *piranha::IrCompilationUnit::resolveLocalName(
    const std::string &name) const 
{
    const int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        IrNode *node = m_nodes[i];
        if (node->getName() == name) {
            return node;
        }
    }

    return nullptr;
}

int piranha::IrCompilationUnit::countSymbolIncidence(const std::string &name) const {
    int count = 0;
    const int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        IrNode *node = m_nodes[i];
        if (!name.empty() && node->getName() == name) {
            ++count;
        }
    }

    return count;
}

void piranha::IrCompilationUnit::addCompilationError(CompilationError *err) {
    if (m_errorList != nullptr) {
        err->setCompilationUnit(this);
        m_errorList->addCompilationError(err);
    }
    else delete FTRACK(err);
}

std::ostream& piranha::IrCompilationUnit::print(std::ostream &stream) {
    return stream;
}

void piranha::IrCompilationUnit::addDependency(IrCompilationUnit *unit) { 
    m_dependencies.push_back(unit); 
}
