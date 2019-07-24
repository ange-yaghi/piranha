#include "../include/language_rules.h"

#include "../include/rule.h"
#include "../include/node.h"
#include "../include/ir_node.h"
#include "../include/ir_context_tree.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_value_constant.h"

#include <assert.h>

piranha::LanguageRules::LanguageRules() {
    /* void */
}

piranha::LanguageRules::~LanguageRules() {
    /* void */
}

piranha::Node *piranha::LanguageRules::generateNode(const std::string &builtinName) const {
    return generateBuiltinType(builtinName);
}

std::string piranha::LanguageRules::resolveOperatorBuiltinType(
    IrBinaryOperator::OPERATOR op, const ChannelType *left, const ChannelType *right) const
{
    std::string *rule = m_operatorRules.lookup({ op, left, right });
    return (rule == nullptr)
        ? ""
        : *rule;
}

std::string piranha::LanguageRules::resolveUnaryOperatorBuiltinType(
    IrUnaryOperator::OPERATOR op, const ChannelType *type) const
{
    std::string *rule = m_unaryOperatorRules.lookup({ op, type });
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
    IrBinaryOperator::OPERATOR op, const ChannelType *left, const ChannelType *right) 
{
    std::string *builtinType = m_operatorRules.lookup({ op, left, right });
    if (builtinType == nullptr) return nullptr;

    Node *node = generateBuiltinType(*builtinType);
    assert(node != nullptr);

    return node;
}

piranha::Node *piranha::LanguageRules::generateUnaryOperator(
    IrUnaryOperator::OPERATOR op, const ChannelType *type)
{
    std::string *builtinType = m_unaryOperatorRules.lookup({ op, type });
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

	Node *newNode = rule->buildNode();
	newNode->setBuiltinName(typeName);

	return newNode;
}

bool piranha::LanguageRules::checkConversion(const NodeTypeConversion &conversion) const {
    return m_conversionRules.lookup(conversion) != nullptr;
}

piranha::Node *piranha::LanguageRules::generateConversion(const NodeTypeConversion &conversion) {
    std::string rule = resolveConversionBuiltinType(conversion);
    if (rule.empty()) return nullptr;

    return generateBuiltinType(rule);
}

std::string piranha::LanguageRules::resolveConversionBuiltinType(
    const NodeTypeConversion &conversion) const 
{
    std::string *rule = m_conversionRules.lookup(conversion);
    if (rule == nullptr) return "";
    else return *rule;
}

bool piranha::LanguageRules::checkBuiltinType(const std::string &builtinType) const {
    BuiltinRule *rule = m_builtinRules.lookup(builtinType);
    return rule != nullptr;
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
