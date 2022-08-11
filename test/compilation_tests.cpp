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

TEST(IrConstructionTests, IrConstructionSanityCheck) {
    MemoryTracker::get()->reset();

    const ErrorList *list;
    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("construction-tests/simple_float.mr", &list, &rules, &compiler);

    EXPECT_EQ(list->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    double value;
    program.getTopLevelContainer()->getNode(1)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 10.0);

    program.getTopLevelContainer()->getNode(3)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 5.0);

    program.getTopLevelContainer()->getNode(5)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 15.0);

    std::string stringValue;
    program.getTopLevelContainer()->getNode(7)->getPrimaryOutput()->fullCompute((void *)&stringValue);
    EXPECT_EQ(stringValue, "5");

    program.getTopLevelContainer()->getNode(9)->getPrimaryOutput()->fullCompute((void *)&stringValue);
    EXPECT_EQ(stringValue, "123");

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrConstructionTests, IrConstructionNestedTest) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileFile("construction-tests/nested_conversions.mr", nullptr, &rules, &compiler);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    Node *topLevel = program.getTopLevelContainer()->getNode(5);
    EXPECT_EQ(topLevel->getContext()->getContext()->getName(), "top_level");

    double value;
    topLevel->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 10.5);

    program.free();
    compiler->free();

    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
