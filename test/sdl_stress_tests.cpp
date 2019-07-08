#include <pch.h>

#include "node.h"
#include "ir_compilation_unit.h"
#include "ir_node.h"
#include "ir_attribute_list.h"
#include "ir_attribute.h"
#include "ir_value_constant.h"
#include "ir_binary_operator.h"
#include "ir_import_statement.h"
#include "ir_node_definition.h"
#include "ir_attribute_definition.h"
#include "ir_attribute_definition_list.h"
#include "ir_compilation_error.h"
#include "ir_error_list.h"
#include "compiler.h"
#include "generator.h"

#include "utilities.h"

using namespace piranha;

const IrErrorList *compileFile(const std::string &filename) {
	IrCompiler *compiler = new IrCompiler();
	IrCompilationUnit *unit = compiler->compile(IR_TEST_FILES + filename);
	EXPECT_NE(unit, nullptr);

	const IrErrorList *errors = compiler->getErrorList();

	IrNode *node = unit->getNode(0);
	std::ofstream f(TMP_PATH + std::string("trace.txt"));
	node->writeTraceToFile(f);
	f.close();

	return errors;
}

TEST(IrSyntaxStressTests, IrSyntaxStressTest1) {
	const IrErrorList *errors = compileFile("stress-testing/stress_test_1.mr");

	EXPECT_EQ(errors->getErrorCount(), 13);

	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 50));
	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 51));
	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 52));

	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 54));
	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 55));
	EXPECT_TRUE(findError(errors, IrErrorCode::UnresolvedReference, 56));

	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 60));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 62));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 64));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 66));

	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 68));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 69));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 74));
}

TEST(IrSyntaxStressTests, IrSyntaxNodeArgumentStressTest1) {
	const IrErrorList *errors = compileFile("stress-testing/node_argument_stress_test_1.mr");

	EXPECT_EQ(errors->getErrorCount(), 5);

	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 22, nullptr, true));
	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 23, nullptr));
}

TEST(IrSyntaxStressTests, IrSyntaxDeepErrorIsolated) {
	const IrErrorList *errors = compileFile("stress-testing/deep_error_isolated.mr");

	EXPECT_EQ(errors->getErrorCount(), 1);

	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 3, nullptr, true));
}

TEST(IrSyntaxStressTests, IrSyntaxDeepError) {
	const IrErrorList *errors = compileFile("stress-testing/deep_error.mr");

	EXPECT_EQ(errors->getErrorCount(), 3);

	EXPECT_TRUE(findError(errors, IrErrorCode::UndefinedMember, 27, nullptr, true));
}
