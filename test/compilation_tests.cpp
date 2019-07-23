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

TEST(IrConstructionTests, IrConstructionSanityCheck) {
    IrCompilationUnit *unit = compileFile("construction-tests/simple_float.mr");

    TestRules generator;
    generator.registerBuiltinNodeTypes();
    NodeProgram program;
    program.setRules(&generator);
    unit->build(&program);

    program.execute();
    
    double value;
    program.getNode(1)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 10.0);

    program.getNode(3)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 5.0);

    program.getNode(5)->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 15.0);

    std::string stringValue;
    program.getNode(7)->getPrimaryOutput()->fullCompute((void *)&stringValue);
    EXPECT_EQ(stringValue, "5");

    program.getNode(9)->getPrimaryOutput()->fullCompute((void *)&stringValue);
    EXPECT_EQ(stringValue, "123");
}

TEST(IrConstructionTests, IrConstructionNestedTest) {
    IrCompilationUnit *unit = compileFile("construction-tests/nested_conversions.mr");

    TestRules generator;
    generator.registerBuiltinNodeTypes();
    NodeProgram program;
    program.setRules(&generator);
    unit->build(&program);

    program.execute();

    Node *topLevel = program.getNode(5);
    EXPECT_EQ(topLevel->getContext()->getContext()->getName(), "top_level");

    double value;
    topLevel->getPrimaryOutput()->fullCompute((void *)&value);
    EXPECT_EQ(value, 10.5);
}
