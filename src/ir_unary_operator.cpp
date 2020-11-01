#include "../include/ir_unary_operator.h"

#include "../include/compilation_error.h"
#include "../include/ir_node.h"
#include "../include/node.h"
#include "../include/ir_context_tree.h"
#include "../include/ir_node_definition.h"
#include "../include/language_rules.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_attribute.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_value_constant.h"
#include "../include/memory_tracker.h"

piranha::IrUnaryOperator::IrUnaryOperator(Operator op, IrValue *operand) : IrValue(IrValue::ValueType::UnaryOperation) {
    m_operand = operand;
    m_operator = op;

    registerComponent(operand);
}

piranha::IrUnaryOperator::~IrUnaryOperator() {
    /* void */
}

piranha::IrParserStructure *piranha::IrUnaryOperator::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
    IR_RESET(query);

    IrNode **pNode = m_expansions.lookup(query.inputContext);

    if (pNode == nullptr) {
        IR_DEAD_END();
        return nullptr;
    }
    else if (*pNode == nullptr) {
        IR_FAIL();
        return nullptr;
    }
    else return *pNode;
}

void piranha::IrUnaryOperator::_expand(IrContextTree *context) {
    if (m_rules == nullptr) return;
    bool emptyContext = context->isEmpty();

    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;
    m_operand->expandChain(context);
    IrParserStructure *reference =
        m_operand->getReference(query, &info);

    if (info.failed) return;
    if (info.reachedDeadEnd) {
        if (!info.isFixedType()) return;
        else reference = info.fixedType;
    }

    const ChannelType *type = (!info.isFixedType())
        ? reference->getImmediateChannelType()
        : info.fixedType->getChannelType();

    const std::string builtinType =
        m_rules->resolveUnaryOperatorBuiltinType(m_operator, type);

    if (builtinType.empty()) {
        const bool touchedMainContext = (info.touchedMainContext && !info.isFixedType());

        if (touchedMainContext || emptyContext) {
            getParentUnit()->addCompilationError(
                TRACK(new CompilationError(m_summaryToken, ErrorCode::InvalidOperandTypes, context))
            );
        }
        return;
    }

    int count = 0;
    IrCompilationUnit *parentUnit = getParentUnit();
    IrNodeDefinition *nodeDefinition = parentUnit->resolveBuiltinNodeDefinition(builtinType, &count);

    // Generate the expansion
    IrAttribute *attribute = TRACK(new IrAttribute());
    attribute->setValue(TRACK(new IrInternalReference(m_operand, context)));

    IrAttributeList *attributeList = TRACK(new IrAttributeList());
    attributeList->addAttribute(attribute);

    IrNode *expansion = TRACK(new IrNode());
    expansion->setAttributes(attributeList);
    expansion->setLogicalParent(this);
    expansion->setScopeParent(this);
    expansion->setDefinition(nodeDefinition);
    expansion->setRules(m_rules);
    expansion->expand(context);
    expansion->resolveDefinitions();

    *m_expansions.newValue(context) = expansion;
}
