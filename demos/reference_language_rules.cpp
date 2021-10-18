#include "reference_language_rules.h"

#include "../include/console_input_node.h"
#include "../include/console_output_node.h"

#include "../include/vector_constructor.h"
#include "../include/literal_node.h"
#include "../include/float_conversions.h"
#include "../include/string_conversions.h"
#include "../include/fundamental_types.h"
#include "../include/default_literal_node.h"
#include "../include/operation_node.h"
#include "../include/add_operation_output.h"
#include "../include/subtract_operation_output.h"
#include "../include/divide_operation_output.h"
#include "../include/multiply_operation_output.h"
#include "../include/channel_node.h"
#include "../include/num_negate_operation.h"
#include "../include/bool_negate_operation.h"
#include "../include/no_op_node.h"
#include "../include/throw_runtime_error_node.h"
#include "../include/int_probe.h"

piranha_demo::ReferenceLanguageRules::ReferenceLanguageRules() {
    /* void */
}

piranha_demo::ReferenceLanguageRules::~ReferenceLanguageRules() {
    /* void */
}

void piranha_demo::ReferenceLanguageRules::registerBuiltinNodeTypes() {
    // Builtin Types
    registerBuiltinType<piranha::ChannelNode>(
        "__piranha__int", &piranha::FundamentalType::IntType);
    registerBuiltinType<piranha::ChannelNode>(
        "__piranha__float", &piranha::FundamentalType::FloatType);
    registerBuiltinType<piranha::ChannelNode>(
        "__piranha__string", &piranha::FundamentalType::StringType);
    registerBuiltinType<piranha::VectorSplitNode>(
        "__piranha__vector", &piranha::FundamentalType::VectorType);
    registerBuiltinType<piranha::VectorSplitNode>(
        "__piranha__bool", &piranha::FundamentalType::BoolType);
    registerBuiltinType<piranha::VectorConstructorNode>(
        "__piranha__vector_constructor", &piranha::FundamentalType::VectorType);

    registerBuiltinType<piranha::ThrowRuntimeErrorNode>(
        "__piranha__throw_runtime_error");
    registerBuiltinType<piranha::IntProbeNode>(
        "__piranha__int_probe");

    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_int, piranha::AddOperationNodeOutput>>("__piranha__int_add");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_float, piranha::AddOperationNodeOutput>>("__piranha__float_add");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_string, piranha::AddOperationNodeOutput>>("__piranha__string_add");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_vector, piranha::AddOperationNodeOutput>>("__piranha__vector_add");

    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_int, piranha::SubtractOperationNodeOutput>>("__piranha__int_subtract");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_float, piranha::SubtractOperationNodeOutput>>("__piranha__float_subtract");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_vector, piranha::SubtractOperationNodeOutput>>("__piranha__vector_subtract");

    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_int, piranha::MultiplyOperationNodeOutput>>("__piranha__int_multiply");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_float, piranha::MultiplyOperationNodeOutput>>("__piranha__float_multiply");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_vector, piranha::MultiplyOperationNodeOutput>>("__piranha__vector_multiply");

    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_int, piranha::DivideOperationNodeOutput>>("__piranha__int_divide");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_float, piranha::DivideOperationNodeOutput>>("__piranha__float_divide");
    registerBuiltinType<piranha::OperationNodeSpecialized<
        piranha::native_vector, piranha::AddOperationNodeOutput>>("__piranha__vector_divide");

    // Unary operations
    registerBuiltinType<piranha::NumNegateOperationNode<piranha::native_int>>("__piranha__int_negate");
    registerBuiltinType<piranha::NumNegateOperationNode<piranha::native_float>>("__piranha__float_negate");
    registerBuiltinType<piranha::NumNegateOperationNode<piranha::native_vector>>("__piranha__vector_negate");

    registerBuiltinType<piranha::NoOpNode>("__piranha__int_positive");
    registerBuiltinType<piranha::NoOpNode>("__piranha__float_positive");
    registerBuiltinType<piranha::NoOpNode>("__piranha__vector_positive");

    registerBuiltinType<piranha::BoolNegateOperationNode<piranha::native_bool>>("__piranha__bool_negate");

    registerBuiltinType<piranha::DefaultLiteralStringNode>
        ("__piranha__literal_string", &piranha::FundamentalType::StringType);
    registerBuiltinType<piranha::DefaultLiteralIntNode>
        ("__piranha__literal_int", &piranha::FundamentalType::IntType);
    registerBuiltinType<piranha::DefaultLiteralFloatNode>
        ("__piranha__literal_float", &piranha::FundamentalType::FloatType);
    registerBuiltinType<piranha::DefaultLiteralBoolNode>
        ("__piranha__literal_bool", &piranha::FundamentalType::BoolType);

    registerBuiltinType<piranha::StringToFloatConversionNode>("__piranha__string_to_float");
    registerBuiltinType<piranha::FloatToStringConversionNode>("__piranha__float_to_string");
    registerBuiltinType<piranha::IntToFloatConversionNode>("__piranha__int_to_float");

    registerBuiltinType<piranha::ConsoleOutputNode>("__piranha__console_out");
    registerBuiltinType<piranha::ConsoleInputNode>("__piranha__console_in");

    // Literals
    registerLiteralType(piranha::LiteralType::String, "__piranha__literal_string");
    registerLiteralType(piranha::LiteralType::Integer, "__piranha__literal_int");
    registerLiteralType(piranha::LiteralType::Float, "__piranha__literal_float");
    registerLiteralType(piranha::LiteralType::Boolean, "__piranha__literal_bool");

    // Conversions
    registerConversion(
        { &piranha::FundamentalType::StringType, &piranha::FundamentalType::FloatType },
        "__piranha__string_to_float"
    );
    registerConversion(
        { &piranha::FundamentalType::FloatType, &piranha::FundamentalType::StringType },
        "__piranha__float_to_string"
    );
    registerConversion(
        { &piranha::FundamentalType::IntType, &piranha::FundamentalType::FloatType },
        "__piranha__int_to_float"
    );

    // Operations

    // ADD
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Add, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
        "__piranha__int_add"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Add, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
        "__piranha__float_add"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Add, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
        "__piranha__float_add"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Add, &piranha::FundamentalType::StringType, &piranha::FundamentalType::StringType },
        "__piranha__string_add"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Add, &piranha::FundamentalType::VectorType, &piranha::FundamentalType::VectorType },
        "__piranha__vector_add"
    );

    // SUBTRACT
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Sub, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
        "__piranha__int_subtract"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Sub, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
        "__piranha__float_subtract"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Sub, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
        "__piranha__float_subtract"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Sub, &piranha::FundamentalType::VectorType, &piranha::FundamentalType::VectorType },
        "__piranha__vector_subtract"
    );

    // MULTIPLY
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Mul, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
        "__piranha__int_multiply"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Mul, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
        "__piranha__float_multiply"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Mul, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
        "__piranha__float_multiply"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Mul, &piranha::FundamentalType::VectorType, &piranha::FundamentalType::VectorType },
        "__piranha__vector_multiply"
    );

    // DIVIDE
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Div, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
        "__piranha__int_divide"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Div, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
        "__piranha__float_divide"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Div, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
        "__piranha__float_divide"
    );
    registerOperator(
        { piranha::IrBinaryOperator::Operator::Div, &piranha::FundamentalType::VectorType, &piranha::FundamentalType::VectorType },
        "__piranha__vector_divide"
    );

    // Unary operations

    // POSITIVE
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::Positive, &piranha::FundamentalType::IntType },
        "__piranha__int_positive"
    );
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::Positive, &piranha::FundamentalType::FloatType },
        "__piranha__float_positive"
    );
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::Positive, &piranha::FundamentalType::VectorType },
        "__piranha__vector_positive"
    );

    // NEGATIVE
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::NumericNegate, &piranha::FundamentalType::IntType },
        "__piranha__int_negate"
    );
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::NumericNegate, &piranha::FundamentalType::FloatType },
        "__piranha__float_negate"
    );
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::NumericNegate, &piranha::FundamentalType::VectorType },
        "__piranha__vector_negate"
    );

    // INVERT
    registerUnaryOperator(
        { piranha::IrUnaryOperator::Operator::BoolNegate, &piranha::FundamentalType::BoolType },
        "__piranha__bool_negate"
    );
}
