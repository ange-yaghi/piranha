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
#include <node_program.h>

#include "test_generator.h"
#include "utilities.h"

using namespace piranha;

IrCompilationUnit *compileFile(const std::string &filename) {
	Compiler *compiler = new Compiler();
	IrCompilationUnit *unit = compiler->compile(IR_TEST_FILES + filename);
	EXPECT_NE(unit, nullptr);

	return unit;
}

TEST(IrConstructionTests, IrConstructionSanityCheck) {
	IrCompilationUnit *unit = compileFile("construction-tests/simple_float.mr");

	TestGenerator generator;
	NodeProgram program;
	program.setGenerator(&generator);
	unit->build(&program);

	program.execute();

	Node *node1 = program.getNode(0);
	EXPECT_EQ(node1->getName(), "");
	
	double value;
	node1->getPrimaryOutput()->fullCompute((void *)&value);
	EXPECT_EQ(value, 5.0);
}
