#include "pch.h"

#include "../include/node.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_node.h"
#include "../include/ir_attribute_list.h"
#include "../include/ir_attribute.h"
#include "../include/ir_value_constant.h"
#include "../include/ir_binary_operator.h"
#include "../include/ir_import_statement.h"
#include "../include/ir_node_definition.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_attribute_definition_list.h"
#include "../include/compilation_error.h"
#include "../include/error_list.h"
#include "../include/compiler.h"
#include "../include/language_rules.h"

#include "utilities.h"

using namespace piranha;

const ErrorList *compileFile(const std::string &filename) {
	Compiler *compiler = new Compiler();
	IrCompilationUnit *unit = compiler->compile(IR_TEST_FILES + filename);
	EXPECT_NE(unit, nullptr);

	const ErrorList *errors = compiler->getErrorList();

	IrNode *node = unit->getNode(0);
	std::ofstream f(TMP_PATH + std::string("trace.txt"));
	node->writeTraceToFile(f);
	f.close();

	return errors;
}

TEST(IrSyntaxStressTests, IrSyntaxStressTest1) {
	const ErrorList *errors = compileFile("stress-testing/stress_test_1.mr");

	EXPECT_EQ(errors->getErrorCount(), 13);

	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 50));
	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 51));
	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 52));

	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 54));
	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 55));
	EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 56));

	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 60));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 62));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 64));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 66));

	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 68));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 69));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 74));
}

TEST(IrSyntaxStressTests, IrSyntaxNodeArgumentStressTest1) {
	const ErrorList *errors = compileFile("stress-testing/node_argument_stress_test_1.mr");

	EXPECT_EQ(errors->getErrorCount(), 5);

	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 22, nullptr, true));
	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 23, nullptr));
}

TEST(IrSyntaxStressTests, IrSyntaxDeepErrorIsolated) {
	const ErrorList *errors = compileFile("stress-testing/deep_error_isolated.mr");

	EXPECT_EQ(errors->getErrorCount(), 1);

	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 3, nullptr, true));
}

TEST(IrSyntaxStressTests, IrSyntaxDeepError) {
	const ErrorList *errors = compileFile("stress-testing/deep_error.mr");

	EXPECT_EQ(errors->getErrorCount(), 3);

	EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 27, nullptr, true));
}
