#include <pch.h>

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
#include "../include/node_program.h"

#include "test_rules.h"
#include "utilities.h"

using namespace piranha;

TEST(GeneralTests, GeneralSyntaxTest_1) {
	const ErrorList *errList;
	IrCompilationUnit *unit = compileFile("general-tests/general_syntax_test_1.mr", &errList);

	EXPECT_TRUE(findError(errList, ErrorCode::UnresolvedReference, 18));

	EXPECT_EQ(errList->getErrorCount(), 1);
}

TEST(GeneralTests, GeneralSyntaxTest_2) {
	const ErrorList *errList;
	IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_2.mr", &errList);

	EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 26));

	EXPECT_EQ(errList->getErrorCount(), 1);
}

TEST(GeneralTests, GeneralSyntaxTest_3) {
	const ErrorList *errList;
	IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_3.mr", &errList);

	EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 24));
	EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 26));

	EXPECT_EQ(errList->getErrorCount(), 2);
}

TEST(GeneralTests, GeneralSyntaxTest_4) {
	const ErrorList *errList;
	IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_4.mr", &errList);

	EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 16));
	EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 18));
	EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 19));

	EXPECT_EQ(errList->getErrorCount(), 4);
}

TEST(GeneralTests, GeneralSyntaxTest_5_retired) {
	/* void */
}

TEST(GeneralTests, GeneralSyntaxTest_6) {
	const ErrorList *errList;
	LanguageRules *rules;
	IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_6.mr", &errList, &rules);

	EXPECT_EQ(errList->getErrorCount(), 0);

	NodeProgram program;
	program.setRules(rules);
	rules->setNodeProgram(&program);
	unit->build(&program);
}

TEST(GeneralTests, GeneralSyntaxTest_7) {
	const ErrorList *errList;
	LanguageRules *rules;
	IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_7.mr", &errList, &rules);

	EXPECT_EQ(errList->getErrorCount(), 0);

	NodeProgram program;
	program.setRules(rules);
	rules->setNodeProgram(&program);
	unit->build(&program);
}
