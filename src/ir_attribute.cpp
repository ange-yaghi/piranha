#include "../include/ir_attribute.h"

#include "../include/ir_value.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_node.h"
#include "../include/language_rules.h"
#include "../include/compilation_error.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_context_tree.h"
#include "../include/memory_tracker.h"

piranha::IrAttribute::IrAttribute() {
    m_value = nullptr;
    m_definition = nullptr;
    m_position = -1;
    setVisibility(IrVisibility::Public);
}

piranha::IrAttribute::IrAttribute(const IrTokenInfo_string &name, IrValue *value) {
    m_name = name;
    m_value = value;
    m_position = -1;

    registerToken(&name);
    registerComponent(value);
    setVisibility(IrVisibility::Public);

    m_definition = nullptr;
}

piranha::IrAttribute::IrAttribute(IrValue *value) {
    m_value = value;
    m_position = -1;

    registerComponent(value);
    setVisibility(IrVisibility::Public);

    m_definition = nullptr;
}

piranha::IrAttribute::~IrAttribute() {
    /* void */
}

void piranha::IrAttribute::setValue(IrValue *value) {
    m_value = value;
    registerComponent(value);
}

void piranha::IrAttribute::free() {
    IrParserStructure::free();
}

piranha::IrParserStructure *piranha::IrAttribute::getImmediateReference(
    const IrReferenceQuery &query, IrReferenceInfo *output) 
{
    IR_INFO_OUT(err, nullptr);
    IR_INFO_OUT(newContext, query.inputContext);
    IR_INFO_OUT(failed, false);

    return m_value;
}

void piranha::IrAttribute::_checkTypes(IrContextTree *context) {
    if (m_definition == nullptr) return;

    IrReferenceInfo info;
    IrReferenceQuery query;
    query.inputContext = context;
    query.recordErrors = false;
    IrParserStructure *finalReference = getReference(query, &info);

    if (info.failed) return;
    if (info.reachedDeadEnd) {
        if (!info.isFixedType()) return;
        else finalReference = info.fixedType;
    }

    if (!info.touchedMainContext && !context->isEmpty()) return;
    if (info.isStaticType() && !context->isEmpty()) return;

    IrNodeDefinition *typeDefinition = m_definition->getTypeDefinition();

    if (typeDefinition != nullptr && m_definition->isInput()) {
        IrNode *refAsNode = finalReference->getAsNode();

        if (info.isFixedType()) {
            if (info.fixedType == typeDefinition) return;
        }
        else {
            if (refAsNode != nullptr) {
                IrNodeDefinition *definition = refAsNode->getDefinition()->getAliasType();
                if (definition == nullptr) return;
                if (definition == typeDefinition) return; // Type is confirmed to be correct
            }
        }

        if (m_rules == nullptr) return;

        const ChannelType *type = info.isFixedType()
            ? info.fixedType->getChannelType()
            : finalReference->getImmediateChannelType();
        const ChannelType *expectedType = typeDefinition->getChannelType();

        if (type == expectedType && expectedType != nullptr) return; // No conversion necessary

        const bool validConversion = m_rules->checkConversion(type, expectedType);
        if (validConversion) return;

        IrCompilationUnit *unit = getParentUnit();
        unit->addCompilationError(new CompilationError(*getSummaryToken(),
            ErrorCode::IncompatibleType, context));
    }
}
