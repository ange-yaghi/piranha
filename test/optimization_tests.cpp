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

TEST(OptimizationTests, OptimizationSanityCheck) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_1.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();

    piranha::native_int v_ref;
    Node *firstNode = program.getTopLevelContainer()->getNode(2);
    firstNode->evaluate();
    firstNode->getPrimaryOutput()->fullCompute((void *)&v_ref);

    program.optimize();

    Node *optimizedForm = program.getTopLevelContainer()->getNode(0);

    optimizedForm->evaluate();

    EXPECT_NE(firstNode, optimizedForm);
    EXPECT_TRUE(optimizedForm->hasFlag(Node::META_CONSTANT));
    
    piranha::native_int v_opt;
    optimizedForm->getPrimaryOutput()->fullCompute((void *)&v_opt);
    
    EXPECT_EQ(v_opt, v_ref);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest2) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_2.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();

    piranha::native_int v_opt, v_ref;
    Node *firstNode = program.getTopLevelContainer()->getNode(8);
    firstNode->evaluate();
    firstNode->getPrimaryOutput()->fullCompute((void *)&v_ref);

    program.optimize();

    Node *optimizedForm = program.getTopLevelContainer()->getNode(0);
    optimizedForm->evaluate();

    EXPECT_NE(firstNode, optimizedForm);
    EXPECT_TRUE(optimizedForm->hasFlag(Node::META_CONSTANT));
    
    optimizedForm->getPrimaryOutput()->fullCompute((void *)&v_opt);
    EXPECT_EQ(v_opt, v_ref);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest3) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_3.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest4) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_4.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest5) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_5.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest6) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_6.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest7) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_7.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
