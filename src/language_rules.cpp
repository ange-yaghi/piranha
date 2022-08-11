#include "../include/language_rules.h"

#include "../include/rule.h"
#include "../include/node.h"
#include "../include/ir_node.h"
#include "../include/ir_context_tree.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_value_constant.h"
#include "../include/channel_node.h"

#include <assert.h>

piranha::LanguageRules::LanguageRules() {
    m_nodeAllocator = &m_defaultNodeAllocator;
}

piranha::LanguageRules::~LanguageRules() {
    const int ruleCount = m_builtinRules.getEntryCount();
    for (int i = 0; i < ruleCount; ++i) {
        m_builtinRules.get(i)->destroy(m_nodeAllocator);
    }
}

void piranha::LanguageRules::initialize() {
    registerBuiltinNodeTypes();
}

piranha::Node *piranha::LanguageRules::generateNode(const std::string &builtinName) const {
    return generateBuiltinType(builtinName);
}

std::string piranha::LanguageRules::resolveOperatorBuiltinType(
    IrBinaryOperator::Operator op, const ChannelType *left, const ChannelType *right) const
{
    if (left == nullptr) return "";
    if (right == nullptr) return "";

    std::string *rule = m_operatorRules.lookup({ op, left, right, false, false });
    return (rule == nullptr)
        ? ""
        : *rule;
}

std::string piranha::LanguageRules::resolveUnaryOperatorBuiltinType(
    IrUnaryOperator::Operator op, const ChannelType *type) const
{
    if (type == nullptr) return "";

    std::string *rule = m_unaryOperatorRules.lookup({ op, type, false });
    return (rule == nullptr)
        ? ""
        : *rule;
}

const piranha::ChannelType *piranha::LanguageRules::resolveChannelType(
    const std::string &builtinName) const
{
    BuiltinRule *rule = m_builtinRules.lookup(builtinName);
    return (rule == nullptr)
        ? nullptr
        : rule->getValue().nodeType;
}

piranha::Node *piranha::LanguageRules::generateOperator(
    IrBinaryOperator::Operator op, const ChannelType *left, const ChannelType *right)
{
    if (left == nullptr) return nullptr;
    if (right == nullptr) return nullptr;

    std::string *builtinType = m_operatorRules.lookup({ op, left, right, false, false });
    if (builtinType == nullptr) return nullptr;

    Node *node = generateBuiltinType(*builtinType);
    assert(node != nullptr);

    return node;
}

piranha::Node *piranha::LanguageRules::generateUnaryOperator(
    IrUnaryOperator::Operator op, const ChannelType *type)
{
    if (type == nullptr) return nullptr;

    std::string *builtinType = m_unaryOperatorRules.lookup({ op, type, false });
    if (builtinType == nullptr) return nullptr;

    Node *node = generateBuiltinType(*builtinType);
    assert(node != nullptr);

    return node;
}

piranha::Node *piranha::LanguageRules::
    generateBuiltinType(const std::string &typeName) const
{
    BuiltinRule *rule = m_builtinRules.lookup(typeName);
    if (rule == nullptr) return nullptr;

    Node *newNode = rule->buildNode(getNodeAllocator());
    newNode->setBuiltinName(typeName);
    newNode->setMemorySpace(Node::MemorySpace::ClientExternal);

    return newNode;
}

bool piranha::LanguageRules::checkConversion(const ChannelType *input, const ChannelType *output) const {
    if (input == nullptr) return false;
    if (output == nullptr) return false;

    return m_conversionRules.lookup( {input, output, false} ) != nullptr;
}

piranha::Node *piranha::LanguageRules::generateConversion(const ChannelType *input, const ChannelType *output) {
    std::string rule = resolveConversionBuiltinType(input, output);
    if (rule.empty()) return nullptr;

    return generateBuiltinType(rule);
}

std::string piranha::LanguageRules::resolveConversionBuiltinType(
    const ChannelType *input, const ChannelType *output) const
{
    if (input == nullptr) return "";
    if (output == nullptr) return "";

    std::string *rule = m_conversionRules.lookup({ input, output, false });
    if (rule == nullptr) return "";
    else return *rule;
}

bool piranha::LanguageRules::checkBuiltinType(const std::string &builtinType) const {
    BuiltinRule *rule = m_builtinRules.lookup(builtinType);
    return rule != nullptr;
}

const piranha::Node *piranha::LanguageRules::getReferenceNode(const std::string &builtinType) const {
    BuiltinRule *rule = m_builtinRules.lookup(builtinType);
    return rule->getReference();
}

void piranha::LanguageRules::registerLiteralType(
    LiteralType literalType, const std::string &builtinType)
{
    *m_literalRules.newValue<std::string>(literalType) = builtinType;
}

void piranha::LanguageRules::registerConversion(
    const NodeTypeConversion &conversion, const std::string &builtinType)
{
    *m_conversionRules.newValue(conversion) = builtinType;
}

void piranha::LanguageRules::registerOperator(
    const OperatorMapping &op, const std::string &builtinType)
{
    *m_operatorRules.newValue<std::string>(op) = builtinType;
}

void piranha::LanguageRules::registerUnaryOperator(
    const UnaryOperatorMapping &op, const std::string &builtinType)
{
    *m_unaryOperatorRules.newValue<std::string>(op) = builtinType;
}
