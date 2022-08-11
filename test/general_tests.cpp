#include <gtest/gtest.h>

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
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileFile("general-tests/general_syntax_test_1.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnresolvedReference, 18));
    EXPECT_EQ(errList->getErrorCount(), 1);

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_02) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_2.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 26));
    EXPECT_EQ(errList->getErrorCount(), 1);

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_03) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_3.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 24));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 26));

    EXPECT_EQ(errList->getErrorCount(), 2);

    compiler->free();
}

TEST(GeneralTests, GeneralSyntaxTest_04) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_4.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 16));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 18));
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 19));

    EXPECT_EQ(errList->getErrorCount(), 4);

    compiler->free();
}

TEST(GeneralTests, GeneralSyntaxTest_05_retired) {
    /* void */
}

TEST(GeneralTests, GeneralSyntaxTest_06) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_6.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_07) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_7.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_08) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_8.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);
}

TEST(GeneralTests, GeneralSyntaxTest_09) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_9.mr", &errList, &rules, &compiler);

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

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_10) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_10.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 5, nullptr, true));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_11) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_11.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 37, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 40, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 33, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 3);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_12) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_12.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 3);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 11, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 13, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 7, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_13) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_13.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 8, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_14) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_14.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 6, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_15) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_15.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedMember, 10, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_16) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_16.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 13, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_17) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_17.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_18) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_18.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 1);
    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinType, 1, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_19) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_19.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 3);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 7, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 27, nullptr, false));

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_20) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_20.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 7, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 12, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 16, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 26, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 4);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_21) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 29, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 31, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 25, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 30, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 32, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 46, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 8);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_21a) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21a.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 15, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 2);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_21b) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_21b.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 14, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 16, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 17, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 3);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_22) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_22.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UnexpectedToken, 17, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleDefaultType, 33, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 38, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::IncompatibleType, 48, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::OutputWithNoDefinition, 7, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 5);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_23) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_23.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinInput, 2, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::UndefinedBuiltinOutput, 4, nullptr, false));

    EXPECT_EQ(errList->getErrorCount(), 5);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_24) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_24a.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_25) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_25.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    piranha::native_int v;
    program.getTopLevelContainer()->getNode(1)->getPrimaryOutput()->fullCompute((void *)&v);

    EXPECT_EQ(v, 15);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_26) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_26.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    EXPECT_FALSE(program.getTopLevelContainer()->getNode(1)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(2)->isEnabled());
    EXPECT_TRUE(program.getTopLevelContainer()->getNode(4)->isEnabled());

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_27) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_27.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    EXPECT_FALSE(program.getTopLevelContainer()->getNode(1)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(2)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(3)->isEnabled());

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_28) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_28.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    EXPECT_FALSE(program.getTopLevelContainer()->getNode(1)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(2)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(3)->isEnabled());

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_29) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_29.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_30) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_30.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    EXPECT_TRUE(program.getTopLevelContainer()->getNode(7)->isEnabled());
    EXPECT_FALSE(program.getTopLevelContainer()->getNode(9)->isEnabled());

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_31) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_31.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_32) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_32.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_33) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_33.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_34) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_34.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_35) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_35.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_36) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_36.mr", &errList, &rules, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::InputSpecifiedMultipleTimes, 12));
    EXPECT_EQ(errList->getErrorCount(), 2);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(GeneralTests, GeneralSyntaxTest_37) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("general-tests/general_syntax_test_37.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
