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

TEST(GeneralTests, GeneralSyntaxTest_01) {
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("general-tests/general_syntax_test_1.mr", &errList);

    EXPECT_TRUE(findError(errList, ErrorCode::UnresolvedReference, 18));

    EXPECT_EQ(errList->getErrorCount(), 1);
}

TEST(GeneralTests, GeneralSyntaxTest_02) {
    const ErrorList *errList;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_2.mr", &errList);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 26));

    EXPECT_EQ(errList->getErrorCount(), 1);
}

TEST(GeneralTests, GeneralSyntaxTest_03) {
    const ErrorList *errList;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_3.mr", &errList);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 24));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 26));

    EXPECT_EQ(errList->getErrorCount(), 2);
}

TEST(GeneralTests, GeneralSyntaxTest_04) {
    const ErrorList *errList;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_4.mr", &errList);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 16));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 18));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 19));

    EXPECT_EQ(errList->getErrorCount(), 4);
}

TEST(GeneralTests, GeneralSyntaxTest_05_retired) {
    /* void */
}

TEST(GeneralTests, GeneralSyntaxTest_06) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_6.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);
}

TEST(GeneralTests, GeneralSyntaxTest_07) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_7.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);
}

TEST(GeneralTests, GeneralSyntaxTest_08) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_8.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);
}

TEST(GeneralTests, GeneralSyntaxTest_09) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_9.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    piranha::vector v;
    program.getTopLevelContainer()->getNode(18)->getPrimaryOutput()->fullCompute(&v);

    EXPECT_EQ(v.x, 0.0);
    EXPECT_EQ(v.y, 0.0);
    EXPECT_EQ(v.z, 0.0);
    EXPECT_EQ(v.w, 10.0);
}

TEST(GeneralTests, GeneralSyntaxTest_10) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_10.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 5, nullptr, true));
}

TEST(GeneralTests, GeneralSyntaxTest_11) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_11.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 24, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 27, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 20, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 3);
}

TEST(GeneralTests, GeneralSyntaxTest_12) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_12.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 3);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 11, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 13, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 7, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_13) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_13.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 8, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_14) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_14.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 6, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_15) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_15.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);

    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedMember, 10, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_16) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_16.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 13, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_17) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_17.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();
}

TEST(GeneralTests, GeneralSyntaxTest_18) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_18.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 1);
    
    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinType, 1, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_19) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_19.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 3);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 7, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 27, nullptr, false));
}

TEST(GeneralTests, GeneralSyntaxTest_20) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_20.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 7, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 12, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 16, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 26, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 4);
}

TEST(GeneralTests, GeneralSyntaxTest_21) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 29, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 31, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 25, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 30, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 32, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 46, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 8);
}

TEST(GeneralTests, GeneralSyntaxTest_21a) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21a.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 15, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 2);
}

TEST(GeneralTests, GeneralSyntaxTest_21b) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21b.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 14, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 16, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 3);
}

TEST(GeneralTests, GeneralSyntaxTest_22) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_22.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 33, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 38, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 48, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::OutputWithNoDefinition, 7, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 5);
}

TEST(GeneralTests, GeneralSyntaxTest_23) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_23.mr", &errList, &rules);

    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinInput, 2, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinOutput, 4, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 5);
}

TEST(GeneralTests, GeneralSyntaxTest_24) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_24a.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_25) {
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_25.mr", &errList, &rules);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    piranha::native_int v;
    program.getTopLevelContainer()->getNode(1)->getPrimaryOutput()->fullCompute((void *)&v);

    EXPECT_EQ(v, 15);

    int a = 0;
}
