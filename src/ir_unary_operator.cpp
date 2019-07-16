#include "../include/ir_unary_operator.h"

#include "../include/compilation_error.h"
#include "../include/ir_node.h"
#include "../include/node.h"
#include "../include/ir_context_tree.h"

piranha::IrUnaryOperator::IrUnaryOperator(OPERATOR op, IrValue *operand) : IrValue(IrValue::UNARY_OPERATION) {
	m_operand = operand;
	m_operator = op;

	registerComponent(operand);
}

piranha::IrUnaryOperator::~IrUnaryOperator() {
	/* void */
}

piranha::IrParserStructure *piranha::IrUnaryOperator::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	IR_RESET(query);

	IrReferenceInfo basicInfo;
	IrReferenceQuery basicQuery;
	basicQuery.inputContext = query.inputContext;
	basicQuery.recordErrors = false;
	IrParserStructure *resolvedOperand = m_operand->getReference(basicQuery, &basicInfo);

	if (IR_FAILED(&basicInfo) || resolvedOperand == nullptr) {
		IR_FAIL();
		return nullptr;
	}

	if (basicInfo.touchedMainContext) IR_INFO_OUT(touchedMainContext, true);

	bool isValidError = (IR_EMPTY_CONTEXT() || basicInfo.touchedMainContext);

	if (m_operator == DEFAULT) {
		IrParserStructure *result = resolvedOperand;

		if (basicInfo.reachedDeadEnd) {
			// This means that this references an input point with no default value. Obviously
			// it makes no sense to check for further errors.
			IR_DEAD_END();
			return nullptr;
		}

		IrNode *asNode = resolvedOperand->getAsNode();
		if (asNode != nullptr) result = asNode->getDefaultPort();

		if (result == nullptr) {
			IR_FAIL();

			if (query.recordErrors && isValidError) {
				// This object does not have a default value
				IR_ERR_OUT(new CompilationError(*m_operand->getSummaryToken(),
					ErrorCode::CannotFindDefaultValue, query.inputContext));
			}

			return nullptr;
		}

		if (query.inputContext != nullptr) IR_INFO_OUT(newContext, query.inputContext->newChild(asNode));

		return result;
	}

	// Shouldn't reach here
	IR_FAIL();
	return nullptr;
}

piranha::NodeOutput *piranha::IrUnaryOperator::_generateNodeOutput(IrContextTree *context, NodeProgram *program) {
	IrValue *resolvedOperand = m_operand;

	if (resolvedOperand == nullptr) return nullptr;

	Node *node = generateNode(context, program);
	IrNode *asNode = resolvedOperand->getAsNode();
	IrValue *value = resolvedOperand->getAsValue();
	
	if (m_operator == DEFAULT) {
		return value->generateNode(context, program)->getPrimaryOutput();
	}

	return nullptr;
}

piranha::Node *piranha::IrUnaryOperator::_generateNode(IrContextTree *context, NodeProgram *program) {
	IrValue *resolvedOperand = m_operand;

	if (resolvedOperand == nullptr) return nullptr;

	IrValue *value = resolvedOperand->getAsValue();
	Node *node = value->generateNode(context, program);

	if (m_operator == DEFAULT) {
		return nullptr;
	}

	return nullptr;
}
