#include "../include/ir_node.h"

#include "../include/ir_attribute.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_attribute_definition.h"
#include "../include/compilation_error.h"
#include "../include/ir_attribute_definition_list.h"
#include "../include/ir_value.h"
#include "../include/ir_context_tree.h"
#include "../include/language_rules.h"
#include "../include/node.h"
#include "../include/standard_allocator.h"
#include "../include/exceptions.h"
#include "../include/node_program.h"

piranha::IrNode::IrNode() {
    m_attributes = nullptr;
    m_isInterface = false;
    m_definition = nullptr;
}

piranha::IrNode::IrNode(
    const IrTokenInfo_string &type, const IrTokenInfo_string &name, 
    IrAttributeList *attributes, const IrTokenInfo_string &library) 
{
    m_type = type;
    m_name = name;
    m_attributes = attributes;
    m_library = library;

    registerToken(&type);
    registerToken(&name);
    registerToken(&library);
    registerComponent(attributes);

    m_definition = nullptr;
    m_isInterface = false;
}

piranha::IrNode::IrNode(
    const IrTokenInfo_string &type, IrAttributeList *attributes, 
    const IrTokenInfo_string &library) 
{
    m_type = type;
    m_name = IrTokenInfo_string("");
    m_attributes = attributes;
    m_library = library;

    registerToken(&type);
    registerToken(&library);
    registerComponent(attributes);

    m_definition = nullptr;
    m_isInterface = false;
}

piranha::IrNode::~IrNode() {
    /* void */
}

const piranha::ChannelType *piranha::IrNode::getImmediateChannelType() {
    return (m_definition != nullptr)
        ? m_definition->getChannelType()
        : nullptr;
}

void piranha::IrNode::setAttributes(IrAttributeList *list) {
    m_attributes = list;
    registerComponent(list);
}

piranha::IrAttribute *piranha::IrNode::getAttribute(const std::string &name, int *count) const {
    if (m_attributes == nullptr) return nullptr;

    if (count != nullptr) *count = 0;

    IrAttribute *firstResult = nullptr;
    int attributeCount = m_attributes->getAttributeCount();
    for (int i = 0; i < attributeCount; i++) {
        IrAttribute *attribute = m_attributes->getAttribute(i);
        bool definitionMatches = attribute->getAttributeDefinition() != nullptr
            ? attribute->getAttributeDefinition()->getName() == name
            : false;
        if (definitionMatches || attribute->getName() == name) {
            if (firstResult == nullptr) firstResult = attribute;
            if (count != nullptr) (*count)++;
        }
    }

    return firstResult;
}

void piranha::IrNode::setScopeParent(IrParserStructure *parentScope) {
    IrParserStructure::setScopeParent(parentScope);
    if (m_attributes != nullptr) {
        m_attributes->setScopeParent(parentScope);
    }
}

piranha::IrAttribute *piranha::IrNode::getAttribute(
    IrAttributeDefinition *definition, int *count) const 
{
    if (m_attributes == nullptr) return nullptr;
    if (count != nullptr) *count = 0;

    IrAttribute *firstResult = nullptr;
    int attributeCount = m_attributes->getAttributeCount();
    for (int i = 0; i < attributeCount; i++) {
        IrAttribute *attribute = m_attributes->getAttribute(i);
        if (attribute->getAttributeDefinition() == definition) {
            if (firstResult == nullptr) firstResult = attribute;
            if (count != nullptr) (*count)++;
        }
    }

    return firstResult;
}

piranha::IrParserStructure *piranha::IrNode::
    resolveLocalName(const std::string &name) const 
{
    IrParserStructure *attribute = getAttribute(name);
    if (attribute != nullptr) return attribute;
    
    if (m_definition != nullptr) return m_definition->resolveLocalName(name);
    else return nullptr;
}

void piranha::IrNode::_resolveDefinitions() {
    resolveNodeDefinition();
    resolveAttributeDefinitions();
}

void piranha::IrNode::_validate() {
    IrAttributeList *attributes = m_attributes;
    IrCompilationUnit *unit = getParentUnit();

    if (attributes == nullptr) {
        // There was a syntax error before this step
        return;
    }

    // Check for symbols used more than once
    int attributeCount = attributes->getAttributeCount();
    for (int i = 0; i < attributeCount; i++) {
        IrAttribute *attribute = attributes->getAttribute(i);

        int count;
        bool positional = attribute->isPositional();

        IrAttributeDefinition *definition = attribute->getAttributeDefinition();

        // If there was an error resolving the definition, skip this validation step
        if (definition != nullptr) getAttribute(definition, &count);
        else continue;

        if (count > 1) {
            // Attribute defined multiple times

            if (positional) {
                // Log a more specific message for clarify if the attribute is positional
                unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(),
                    ErrorCode::InputSpecifiedMultipleTimesPositional));
            }
            else {
                unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(),
                    ErrorCode::InputSpecifiedMultipleTimes));
            }
        }
    }

    // Check for missing inputs
    if (m_definition != nullptr) {
        auto attributeList = m_definition->getAttributeDefinitionList();

        // Check that there wasn't an error in the definition
        if (attributeList != nullptr) {
            int inputCount = attributeList->getInputCount();

            for (int i = 0; i < inputCount; i++) {
                IrAttributeDefinition *input = attributeList->getInputDefinition(i);
                IrParserStructure *attribute = getAttribute(input);

                if (attribute == nullptr && input->getDefaultValue() == nullptr) {
                    // This input port is not conencted and has no default value
                    unit->addCompilationError(new CompilationError(*getSummaryToken(),
                        ErrorCode::InputNotConnected));
                }
            }
        }
    }
}

void piranha::IrNode::_checkInstantiation() {
    // Check all references relating to the connection of inputs from this
    // node to the actual definition.
    IrContextTree *parentContext = new IrContextTree(nullptr);
    IrContextTree *mainContext = parentContext->newChild(this, true);

    if (m_definition != nullptr) {
        m_definition->checkReferences(mainContext);
    }
}

void piranha::IrNode::_expand() {
    // Expand with this node as the focus                      ____
    IrContextTree *parentContext = new IrContextTree(nullptr); ////
    IrContextTree *mainContext = parentContext->newChild(this, true);
    if (m_definition != nullptr) {
        m_definition->expand(mainContext);
    }
}

void piranha::IrNode::_expand(IrContextTree *context) {
    // Standard expand with a different focus            _____
    IrContextTree *mainContext = context->newChild(this, false);
    if (m_definition != nullptr) {
        m_definition->expand(mainContext);
    }
}

void piranha::IrNode::_checkTypes() {
    // Check types with this node as the focus                   ____
    IrContextTree *parentContext = new IrContextTree(nullptr); ////
    IrContextTree *mainContext = parentContext->newChild(this, true);
    if (m_definition != nullptr) {
        m_definition->checkTypes(mainContext);
    }
}

void piranha::IrNode::_checkTypes(IrContextTree *context) {
    // Standard type check with a different focus        _____
    IrContextTree *mainContext = context->newChild(this, false);
    if (m_definition != nullptr) {
        m_definition->checkTypes(mainContext);
    }
}

void piranha::IrNode::resolveNodeDefinition() {
    // In case the definition for this node has been specified beforehand
    if (m_definition != nullptr) return;

    int definitionCount = 0;
    IrNodeDefinition *definition = nullptr;
    IrCompilationUnit *unit = getParentUnit();

    if (m_library.specified) {
        if (!m_library.data.empty()) {
            definition = unit->resolveNodeDefinition(getType(), &definitionCount, m_library.data);
        }
        else {
            // Adding an empty library name means that the local scope must strictly be used
            definition = unit->resolveLocalNodeDefinition(getType(), &definitionCount);
        }
    }
    else definition = unit->resolveNodeDefinition(getType(), &definitionCount, "");

    if (definitionCount > 0) {
        // TODO: log a warning when a node type is ambiguous
    }

    if (definition == nullptr) {
        unit->addCompilationError(new CompilationError(getTypeToken(), 
            ErrorCode::UndefinedNodeType));
    }
    else setDefinition(definition);
}

void piranha::IrNode::resolveAttributeDefinitions() {
    if (m_definition == nullptr) {
        // The definition wasn't found so resolving any attributes doesn't make sense
        return;
    }

    if (m_attributes == nullptr) {
        // There was a compilation error in the attributes section, so this step can be skipped
        return;
    }

    IrCompilationUnit *unit = getParentUnit();

    int attributeCount = m_attributes->getAttributeCount();
    for (int i = 0; i < attributeCount; i++) {
        IrAttribute *attribute = m_attributes->getAttribute(i);

        IrAttributeDefinition *definition;
        
        if (attribute->isPositional()) {
            const IrAttributeDefinitionList *list = m_definition->getAttributeDefinitionList();
            int position = attribute->getPosition();

            // Check position is not out of bounds
            if (position >= list->getInputCount()) {
                unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
                    ErrorCode::ArgumentPositionOutOfBounds));
                attribute->setAttributeDefinition(nullptr);
                return;
            }
            else {
                definition = m_definition->getAttributeDefinitionList()->getInputDefinition(
                    attribute->getPosition()
                );
            }
        }
        else definition = m_definition->getAttributeDefinition(attribute->getName());

        if (definition == nullptr) {
            // Port not found
            unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
                ErrorCode::PortNotFound));
            attribute->setAttributeDefinition(nullptr);
        }
        else if (definition->getDirection() == IrAttributeDefinition::OUTPUT) {
            // Can't assign an output port
            unit->addCompilationError(new CompilationError(*attribute->getSummaryToken(), 
                ErrorCode::UsingOutputPortAsInput));
            attribute->setAttributeDefinition(nullptr);
        }
        else {
            // Port is good
            attribute->setAttributeDefinition(definition);
        }
    }
}

piranha::Node *piranha::IrNode::_generateNode(IrContextTree *context, NodeProgram *program) {
    if (isInterface()) {
        Node *node = getScopeParent()
            ->_generateNodeOutput(context, program)
            ->getInterface();

        if (node != nullptr) node->setBuiltinName(m_definition->getBuiltinName());

        return node;
    }

    IrContextTree *newContext;
    IrContextTree *parentContext = context;
    newContext = parentContext->newChild(this);

    IrNodeDefinition *definition = getDefinition();
    const IrAttributeDefinitionList *allAttributes = definition->getAttributeDefinitionList();
    const IrAttributeList *specifiedAttributes = getAttributes();
    
    struct Mapping {
        std::string name;
        NodeOutput *output;
    };

    std::vector<Mapping> inputs;

    int attributeCount = allAttributes->getDefinitionCount();
    for (int i = 0; i < attributeCount; i++) {
        IrAttributeDefinition *attributeDefinition = allAttributes->getDefinition(i);

        IrReferenceInfo info;
        IrReferenceQuery query;
        query.inputContext = newContext;
        query.recordErrors = false;
        IrParserStructure *reference = attributeDefinition->getReference(query, &info);

        // Skip if this is a builtin output as it wouldn't even be generated yet
        if (attributeDefinition->getDirection() == IrAttributeDefinition::OUTPUT &&
            m_definition->isBuiltin()) continue;

        NodeOutput *output = reference->generateNodeOutput(info.newContext, program);

        if (attributeDefinition->isInput()) {
            inputs.push_back({ attributeDefinition->getName(), output });
        }
    }

    // Generate internal nodes
    IrNodeList *nestedNodeList = definition->getBody();
    if (nestedNodeList != nullptr) {
        int nestedNodeCount = nestedNodeList->getItemCount();
        for (int i = 0; i < nestedNodeCount; i++) {
            IrNode *node = nestedNodeList->getItem(i);
            node->generateNode(newContext, program);
        }
    }

    std::string builtinType = m_definition->getBuiltinName();
    if (builtinType.empty()) return nullptr;

    Node *newNode = m_rules->generateNode(builtinType);
    if (newNode != nullptr) {
        newNode->setName(getName());
        newNode->initialize();

        int inputCount = (int)inputs.size();
        for (int i = 0; i < inputCount; i++) {
            newNode->connectInput(inputs[i].output, inputs[i].name.c_str());
        }
    }
    else {
        // The type is not valid
        // If errors are checked properly
    }

    return newNode;
}

piranha::NodeOutput *piranha::IrNode::_generateNodeOutput(
    IrContextTree *context, NodeProgram *program) 
{
    return isInterface()
        ? getScopeParent()->generateNodeOutput(context, program)
        : nullptr;
}

piranha::IrParserStructure *piranha::IrNode::getDefaultPort(bool *failed) {
    auto definition = m_definition;
    if (definition == nullptr) {
        *failed = true;
        return nullptr;
    }

    auto definitionList = definition->getAttributeDefinitionList();
    if (definitionList == nullptr) {
        *failed = true;
        return nullptr;
    }

    *failed = false;
    return definitionList->getAliasOutput();
}

void piranha::IrNode::writeTraceToFile(std::ofstream &file) {
    IrContextTree *parentContext = new IrContextTree(nullptr);
    IrContextTree *thisContext = parentContext->newChild(this);

    int attributeDefinitions = m_definition->getAttributeDefinitionList()->getDefinitionCount();
    for (int i = 0; i < attributeDefinitions; i++) {
        m_definition
            ->getAttributeDefinitionList()
            ->getDefinition(i)
            ->writeReferencesToFile(file, thisContext);
    }
}

piranha::IrParserStructure *piranha::IrNode::getImmediateReference(
    const IrReferenceQuery &query, IrReferenceInfo *output) 
{
    IR_RESET(query);

    bool failed = false;
    IrParserStructure *aliasPort = getDefaultPort(&failed);

    if (failed) {
        IR_FAIL();
        return nullptr;
    }

    if (aliasPort != nullptr) {
        IrContextTree *newContext = query.inputContext->newChild(this, false);
        IR_INFO_OUT(newContext, newContext);
    }

    return aliasPort;
}

void piranha::IrNode::checkReferences(IrContextTree *inputContext) {
    IrContextTree *thisContext = inputContext->newChild(this, false);
    IrAttributeList *attributes = getAttributes();

    if (attributes != nullptr) {
        int attributeCount = attributes->getAttributeCount();
        for (int i = 0; i < attributeCount; i++) {
            attributes->getAttribute(i)->checkReferences(inputContext);
        }
    }

    if (m_definition != nullptr) {
        m_definition->checkReferences(thisContext);
    }
}
