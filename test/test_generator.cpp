#include <pch.h>

#include <test_generator.h>

#include <constructed_float_node.h>
#include <constructed_string_node.h>
#include <literal_node.h>

TestGenerator::TestGenerator() {
	/* void */
}

TestGenerator::~TestGenerator() {
	/* void */
}

void TestGenerator::registerBuiltinNodeTypes() {
	registerBuiltinType<piranha::ConstructedFloatNode>("__piranha__float");
	registerBuiltinType<piranha::ConstructedStringNode>("__piranha__string");

	registerLiteralType<piranha::SpecializedLiteralNode<std::string> >(piranha::IrValue::CONSTANT_STRING);
	registerLiteralType<piranha::SpecializedLiteralNode<int> >(piranha::IrValue::CONSTANT_INT);
	registerLiteralType<piranha::SpecializedLiteralNode<double> >(piranha::IrValue::CONSTANT_FLOAT);
	registerLiteralType<piranha::SpecializedLiteralNode<bool> >(piranha::IrValue::CONSTANT_BOOL);
}
