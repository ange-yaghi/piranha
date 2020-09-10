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

TEST(OptimizationTests, OptimizationSanityCheck) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_1.mr", &errList, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    Node *optimizedForm = program.getTopLevelContainer()->getNode(0);
    Node *firstNode = program.getTopLevelContainer()->getNode(0)->getUnoptimizedForm();

    optimizedForm->evaluate();

    EXPECT_NE(firstNode, optimizedForm);
    EXPECT_TRUE(optimizedForm->hasFlag(Node::META_CONSTANT));
    
    piranha::native_int v_opt, v_ref;
    optimizedForm->getPrimaryOutput()->fullCompute((void *)&v_opt);
    firstNode->getPrimaryOutput()->fullCompute((void *)&v_ref);
    EXPECT_EQ(v_opt, v_ref);

    compiler->free();
    program.free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest2) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_2.mr", &errList, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    Node *optimizedForm = program.getTopLevelContainer()->getNode(0);
    Node *firstNode = program.getTopLevelContainer()->getNode(0)->getUnoptimizedForm();

    optimizedForm->evaluate();

    EXPECT_NE(firstNode, optimizedForm);
    EXPECT_TRUE(optimizedForm->hasFlag(Node::META_CONSTANT));

    piranha::native_int v_opt, v_ref;
    optimizedForm->getPrimaryOutput()->fullCompute((void *)&v_opt);
    firstNode->getPrimaryOutput()->fullCompute((void *)&v_ref);
    EXPECT_EQ(v_opt, v_ref);

    compiler->free();
    program.free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest3) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_3.mr", &errList, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    compiler->free();
    program.free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest4) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_4.mr", &errList, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    compiler->free();
    program.free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(OptimizationTests, OptimizationTest5) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("optimization-tests/optimization_test_5.mr", &errList, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.initialize();
    program.optimize();

    program.execute();

    compiler->free();
    program.free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
