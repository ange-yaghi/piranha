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
	registerBuiltinType<piranha::FloatCastNode>("__piranha__int", &piranha::FundamentalType::IntType);
	registerBuiltinType<piranha::FloatCastNode>("__piranha__float", &piranha::FundamentalType::FloatType);
	registerBuiltinType<piranha::StringCastNode>("__piranha__string", &piranha::FundamentalType::StringType);

	registerBuiltinType<piranha::OperationNodeSpecialized<piranha::native_int>>("__piranha__int_add");
	registerBuiltinType<piranha::OperationNodeSpecialized<piranha::native_float>>("__piranha__float_add");

	registerBuiltinType<piranha::DefaultLiteralStringNode>("__piranha__literal_string", &piranha::FundamentalType::StringType);
	registerBuiltinType<piranha::DefaultLiteralIntNode>("__piranha__literal_int", &piranha::FundamentalType::IntType);
	registerBuiltinType<piranha::DefaultLiteralFloatNode>("__piranha__literal_float", &piranha::FundamentalType::FloatType);
	registerBuiltinType<piranha::DefaultLiteralBoolNode>("__piranha__literal_bool", &piranha::FundamentalType::BoolType);

	// Literals
	registerLiteralType(piranha::LITERAL_STRING, "__piranha__literal_string");
	registerLiteralType(piranha::LITERAL_INT, "__piranha__literal_int");
	registerLiteralType(piranha::LITERAL_FLOAT, "__piranha__literal_float");
	registerLiteralType(piranha::LITERAL_BOOL, "__piranha__literal_bool");

	// Conversions
	registerConversion<piranha::StringToFloatConversionNode>(
		{ &piranha::FundamentalType::StringType, &piranha::FundamentalType::FloatType }
	);
	registerConversion<piranha::FloatToStringConversionNode>(
		{ &piranha::FundamentalType::FloatType, &piranha::FundamentalType::StringType }
	);

	// Operations
	registerOperator(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::IntType, &piranha::FundamentalType::IntType },
		"__piranha__int_add"
	);
	registerOperator(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::IntType },
		"__piranha__float_add"
	);
	registerOperator(
		{ piranha::IrBinaryOperator::ADD, &piranha::FundamentalType::FloatType, &piranha::FundamentalType::FloatType },
		"__piranha__float_add"
	);
}
