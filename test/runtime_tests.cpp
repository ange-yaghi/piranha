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

TEST(RuntimeTests, RuntimeErrorTest) {
    const ErrorList *errList;
    IrCompilationUnit *unit = compileFile("runtime-tests/runtime_error_test.mr", &errList);

    NodeProgram program;
    unit->build(&program);

    bool result = program.execute();
    EXPECT_FALSE(result);

    EXPECT_EQ(program.getRuntimeError(), "Planned error");
}
