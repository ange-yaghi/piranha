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
#include "compilation_error.h"
#include "error_list.h"
#include "compiler.h"
#include "language_rules.h"
#include <node_program.h>

#include "test_rules.h"
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

	TestRules generator;
	NodeProgram program;
	program.setRules(&generator);
	unit->build(&program);

	program.execute();

	Node *node1 = program.getNode(0);
	EXPECT_EQ(node1->getName(), "");

	Node *node2 = program.getNode(2);
	EXPECT_EQ(node2->getName(), "string_to_float");
	
	double value;
	program.getNode(0)->getPrimaryOutput()->fullCompute((void *)&value);
	EXPECT_EQ(value, 10.0);

	program.getNode(1)->getPrimaryOutput()->fullCompute((void *)&value);
	EXPECT_EQ(value, 5.0);

	program.getNode(2)->getPrimaryOutput()->fullCompute((void *)&value);
	EXPECT_EQ(value, 15.0);

	std::string stringValue;
	program.getNode(3)->getPrimaryOutput()->fullCompute((void *)&stringValue);
	EXPECT_EQ(stringValue, "5");

	program.getNode(4)->getPrimaryOutput()->fullCompute((void *)&stringValue);
	EXPECT_EQ(stringValue, "DEFAULT");
}

TEST(IrConstructionTests, IrConstructionNestedTest) {
	IrCompilationUnit *unit = compileFile("construction-tests/nested_conversions.mr");

	TestRules generator;
	NodeProgram program;
	program.setRules(&generator);
	unit->build(&program);

	program.execute();

	Node *topLevel = program.getNode(8);
	EXPECT_EQ(topLevel->getName(), "top_level");

	double value;
	topLevel->getPrimaryOutput()->fullCompute((void *)&value);
	EXPECT_EQ(value, 10.5);
}
