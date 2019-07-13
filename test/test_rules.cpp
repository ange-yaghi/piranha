#include <pch.h>

#include "test_rules.h"

#include "../include/float_cast_node.h"
#include "../include/string_cast_node.h"
#include "../include/literal_node.h"
#include "../include/float_conversions.h"
#include "../include/string_conversions.h"
#include "../include/fundamental_types.h"
#include "../include/default_literal_node.h"
#include "../include/operation_node.h"

TestRules::TestRules() {
	/* void */
}

TestRules::~TestRules() {
	/* void */
}

void TestRules::registerBuiltinNodeTypes() {
	// Builtin Types
	registerBuiltinType<piranha::FloatCastNode>("__piranha__float", &piranha::FundamentalType::FloatType);
	registerBuiltinType<piranha::StringCastNode>("__piranha__string", &piranha::FundamentalType::IntType);
	registerBuiltinType<piranha::OperationNodeSpecialized<piranha::native_int>>("__piranha__int_add");
	registerBuiltinType<piranha::OperationNodeSpecialized<piranha::native_float>>("__piranha__float_add");

	// Literals
	registerLiteralType<piranha::DefaultLiteralStringNode, piranha::LiteralStringType>();
	registerLiteralType<piranha::DefaultLiteralIntNode, piranha::LiteralIntType>();
	registerLiteralType<piranha::DefaultLiteralFloatNode, piranha::LiteralFloatType>();
	registerLiteralType<piranha::DefaultLiteralBoolNode, piranha::LiteralBoolType>();

	// Conversions
	registerConversion<piranha::StringToFloatConversionNode>(
		{ &piranha::FundamentalType::StringType, &piranha::FundamentalType::FloatType }
	);
	registerConversion<piranha::FloatToStringConversionNode>(
		{ &piranha::FundamentalType::FloatType, &piranha::FundamentalType::StringType }
	);

	// Operations
	registerOperator<piranha::OperationNodeSpecialized<piranha::native_int>>(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
		{ "__piranha__int_add" }
	);
	registerOperator<piranha::OperationNodeSpecialized<piranha::native_float>>(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
		{ "__piranha__float_add" }
	);
	registerOperator<piranha::OperationNodeSpecialized<piranha::native_float>>(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
		{ "__piranha__float_add" }
	);
}
