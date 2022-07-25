#include <gtest/gtest.h>

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

#include "utilities.h"

using namespace piranha;

TEST(IrTests, IrSanityCheck) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_empty_node.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");
        EXPECT_EQ(attributes->getAttributeCount(), 0);
        EXPECT_EQ(parser.getNodeCount(), 1);

        CHECK_IR_POS(node, 1, 23, 1, 1);
        CHECK_IR_POS(attributes, 21, 23, 1, 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrSingleAttribute) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_single_attrib.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute = attributes->getAttribute(0);

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");

        EXPECT_EQ(attribute->getName(), "test");
        EXPECT_EQ(attribute->getValue()->getType(), IrValue::ValueType::ConstantLabel);
        EXPECT_EQ(((IrValueLabel *)attribute->getValue())->getValue(), "test");
        EXPECT_EQ(attributes->getAttributeCount(), 1);

        EXPECT_EQ(parser.getNodeCount(), 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrTwoAttributes) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_two_attribs.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute0 = attributes->getAttribute(0);
        const IrAttribute *attribute1 = attributes->getAttribute(1);

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");

        EXPECT_EQ(attribute0->getName(), "test1");
        EXPECT_EQ(attribute0->getValue()->getType(), IrValue::ValueType::ConstantLabel);
        EXPECT_EQ(((IrValueLabel *)attribute0->getValue())->getValue(), "test");

        EXPECT_EQ(attribute1->getName(), "test2");
        EXPECT_EQ(attribute1->getValue()->getType(), IrValue::ValueType::ConstantLabel);
        EXPECT_EQ(((IrValueLabel *)attribute1->getValue())->getValue(), "test");

        EXPECT_EQ(attributes->getAttributeCount(), 2);

        EXPECT_EQ(parser.getNodeCount(), 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrTwoNodes) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "two_nodes.mr");

        const IrNode *node1 = parser.getNode(0);
        const IrAttributeList *attributes1 = node1->getAttributes();

        const IrNode *node2 = parser.getNode(1);
        const IrAttributeList *attributes2 = node2->getAttributes();

        EXPECT_EQ(node1->getName(), "testImage");
        EXPECT_EQ(node1->getType(), "FileImage");
        EXPECT_EQ(attributes1->getAttributeCount(), 2);

        EXPECT_EQ(node2->getName(), "testImage2");
        EXPECT_EQ(node2->getType(), "FileImage");
        EXPECT_EQ(attributes2->getAttributeCount(), 2);

        EXPECT_EQ(parser.getNodeCount(), 2);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrInlineNode) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_inline_node.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute0 = attributes->getAttribute(0);
        const IrAttribute *attribute1 = attributes->getAttribute(1);

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");

        EXPECT_EQ(attribute0->getName(), "test1");
        EXPECT_EQ(attribute0->getValue()->getType(), IrValue::ValueType::ConstantLabel);
        EXPECT_EQ(((IrValueLabel *)attribute0->getValue())->getValue(), "test");

        EXPECT_EQ(attribute1->getName(), "test2");
        EXPECT_EQ(attribute1->getValue()->getType(), IrValue::ValueType::NodeReference);

        IrNode *inlineNode = ((IrValueNodeRef *)attribute1->getValue())->getValue();
        EXPECT_EQ(inlineNode->getType(), "InlineNode");
        EXPECT_EQ(inlineNode->getName(), "");
        EXPECT_EQ(inlineNode->getAttributes()->getAttributeCount(), 1);

        IrNode *inlineNode2 = ((IrValueNodeRef *)attributes->getAttribute(2)->getValue())->getValue();
        EXPECT_EQ(inlineNode2->getType(), "InlineNode2");
        EXPECT_EQ(inlineNode2->getName(), "");
        EXPECT_EQ(inlineNode2->getAttributes()->getAttributeCount(), 1);

        EXPECT_EQ(attributes->getAttributeCount(), 3);

        EXPECT_EQ(parser.getNodeCount(), 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrSimpleIntTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_single_int.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute0 = attributes->getAttribute(0);
        const IrAttribute *attribute1 = attributes->getAttribute(1);

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");

        EXPECT_EQ(attribute0->getName(), "test_dec");
        EXPECT_EQ(attribute0->getValue()->getType(), IrValue::ValueType::ConstantInt);
        EXPECT_EQ(((IrValueInt *)attribute0->getValue())->getValue(), 10);

        EXPECT_EQ(attribute1->getName(), "test_hex_1");
        EXPECT_EQ(attribute1->getValue()->getType(), IrValue::ValueType::ConstantInt);
        EXPECT_EQ(((IrValueInt *)attribute1->getValue())->getValue(), 16);

        EXPECT_EQ(attributes->getAttributeCount(), 2);

        EXPECT_EQ(parser.getNodeCount(), 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrStringSanityCheck) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_string_attrib.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute = attributes->getAttribute(0);

        EXPECT_EQ(node->getName(), "testImage");
        EXPECT_EQ(node->getType(), "FileImage");

        EXPECT_EQ(attribute->getName(), "test");
        EXPECT_EQ(attribute->getValue()->getType(), IrValue::ValueType::ConstantString);
        EXPECT_EQ(((IrValueString *)attribute->getValue())->getValue(), "test");
        EXPECT_EQ(attributes->getAttributeCount(), 1);

        EXPECT_EQ(parser.getNodeCount(), 1);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrSingleNodeDataAccess) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_data_access.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();
        const IrAttribute *attribute0 = attributes->getAttribute(0);
        const IrAttribute *attribute1 = attributes->getAttribute(1);

        const IrValue *v0 = attribute0->getValue();
        const IrValue *v1 = attribute1->getValue();

        EXPECT_EQ(v0->getType(), IrValue::ValueType::BinaryOperation);
        EXPECT_EQ(v1->getType(), IrValue::ValueType::BinaryOperation);

        EXPECT_EQ(((IrBinaryOperator *)v0)->getLeft()->getType(), IrValue::ValueType::NodeReference);
        EXPECT_EQ(((IrBinaryOperator *)v0)->getRight()->getType(), IrValue::ValueType::ConstantLabel);

        EXPECT_EQ(((IrBinaryOperator *)v1)->getLeft()->getType(), IrValue::ValueType::BinaryOperation);
        EXPECT_EQ(((IrBinaryOperator *)v1)->getRight()->getType(), IrValue::ValueType::ConstantLabel);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrSingleNodeSimpleEq) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_simple_eq.mr");

        const IrNode *node = parser.getNode(0);
        const IrAttributeList *attributes = node->getAttributes();

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrImportTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_import_statement.mr");

        int importCount = parser.getImportStatementCount();

        EXPECT_EQ(importCount, 2);

        IrImportStatement *a = parser.getImportStatement(0);
        IrImportStatement *b = parser.getImportStatement(1);

        EXPECT_EQ(a->getLibName(), "test.sdl");
        EXPECT_EQ(b->getLibName(), "test");

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrNodeDefinitionTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_definition.mr");

        EXPECT_EQ(parser.getNodeDefinitionCount(), 1);

        IrNodeDefinition *nodeDef = parser.getNodeDefinition(0);
        EXPECT_EQ(nodeDef->getName(), "NewNode");
        EXPECT_EQ(nodeDef->isBuiltin(), false);
        EXPECT_EQ(nodeDef->getVisibility(), IrVisibility::Public);

        const IrAttributeDefinitionList *definitions =
            nodeDef->getAttributeDefinitionList();

        EXPECT_EQ(definitions->getDefinitionCount(), 4);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrNodeBuiltinTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_builtin.mr");

        EXPECT_EQ(parser.getNodeDefinitionCount(), 1);

        IrNodeDefinition *nodeDef = parser.getNodeDefinition(0);
        EXPECT_EQ(nodeDef->getBuiltinName(), "BuiltinNode");
        EXPECT_EQ(nodeDef->getName(), "NewNode");
        EXPECT_EQ(nodeDef->isBuiltin(), true);
        EXPECT_EQ(nodeDef->getVisibility(), IrVisibility::Public);

        const IrAttributeDefinitionList *definitions =
            nodeDef->getAttributeDefinitionList();

        EXPECT_EQ(definitions->getDefinitionCount(), 4);

        EXPECT_EQ(definitions->getDefinition(0)->isAlias(), true);
        EXPECT_EQ(definitions->getDefinition(0)->getDirection(),
            IrAttributeDefinition::Direction::Input);
        EXPECT_EQ(definitions->getDefinition(0)->getName(), "A");
        CHECK_IR_POS(definitions->getDefinition(0), 5, 18, 19, 19);

        EXPECT_EQ(definitions->getDefinition(1)->isAlias(), false);
        EXPECT_EQ(definitions->getDefinition(1)->getDirection(),
            IrAttributeDefinition::Direction::Input);
        EXPECT_EQ(definitions->getDefinition(1)->getName(), "B");

        EXPECT_EQ(definitions->getDefinition(2)->isAlias(), true);
        EXPECT_EQ(definitions->getDefinition(2)->getDirection(),
            IrAttributeDefinition::Direction::Output);
        EXPECT_EQ(definitions->getDefinition(2)->getName(), "A_out");

        EXPECT_EQ(definitions->getDefinition(3)->isAlias(), false);
        EXPECT_EQ(definitions->getDefinition(3)->getDirection(),
            IrAttributeDefinition::Direction::Output);
        EXPECT_EQ(definitions->getDefinition(3)->getName(), "B_out");

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrFloatTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_vector_float.mr");

        IrNode *node = parser.getNode(0);
        IrAttribute *testAttrib = node->getAttributes()->getAttribute(0);

        EXPECT_EQ(testAttrib->getName(), "test_float");

        IrValue *value = testAttrib->getValue();
        EXPECT_EQ(value->getType(), IrValue::ValueType::NodeReference);

        IrValueNodeRef *nodeRef = static_cast<IrValueNodeRef *>(value);
        node = nodeRef->getValue();
        IrAttribute *attribute1 = node->getAttributes()->getAttribute(0);
        IrAttribute *attribute2 = node->getAttributes()->getAttribute(1);
        EXPECT_EQ(attribute1->getValue()->getType(), IrValue::ValueType::ConstantFloat);
        EXPECT_EQ(attribute2->getValue()->getType(), IrValue::ValueType::ConstantFloat);

        IrValueFloat *v1 = (IrValueFloat *)attribute1->getValue();
        IrValueFloat *v2 = (IrValueFloat *)attribute2->getValue();

        EXPECT_DOUBLE_EQ(v1->getValue(), 10.123);
        EXPECT_DOUBLE_EQ(v2->getValue(), 15.456);

        CHECK_IR_POS(v1, 26, 32, 2, 2);
        CHECK_IR_POS(v2, 34, 40, 2, 2);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrBoolTest) {
    MemoryTracker::get()->reset();
    {
        IrCompilationUnit parser;
        parser.parseFile(IR_TEST_FILES "single_node_bool.mr");

        IrNode *node = parser.getNode(0);
        IrAttribute *falseAttrib = node->getAttributes()->getAttribute(0);
        IrAttribute *trueAttrib = node->getAttributes()->getAttribute(1);

        EXPECT_EQ(falseAttrib->getName(), "test_false");
        EXPECT_EQ(trueAttrib->getName(), "test_true");

        IrValue *falseValue = falseAttrib->getValue();
        IrValue *trueValue = trueAttrib->getValue();

        EXPECT_EQ(falseValue->getType(), IrValue::ValueType::ConstantBool);
        EXPECT_EQ(trueValue->getType(), IrValue::ValueType::ConstantBool);

        IrValueBool *castFalse = (IrValueBool *)falseValue;
        IrValueBool *castTrue = (IrValueBool *)trueValue;

        EXPECT_EQ(castFalse->getValue(), false);
        EXPECT_EQ(castTrue->getValue(), true);

        CHECK_IR_POS(castFalse, 17, 22, 2, 2);
        CHECK_IR_POS(castTrue, 16, 20, 3, 3);

        parser.free();
    }

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrSyntaxErrorTest) {
    MemoryTracker::get()->reset();
    {
        ErrorList errorList;
        IrCompilationUnit parser;
        parser.setErrorList(&errorList);
        parser.parseFile(IR_TEST_FILES "syntax_error.mr");

        int errorCount = errorList.getErrorCount();
        EXPECT_EQ(errorCount, 3);

        CompilationError *err1 = errorList.getCompilationError(0);
        CompilationError *err2 = errorList.getCompilationError(1);
        CompilationError *err3 = errorList.getCompilationError(2);

        EXPECT_ERROR_CODE(err1, ErrorCode::UnidentifiedToken);
        EXPECT_ERROR_CODE(err2, ErrorCode::UnidentifiedToken);
        EXPECT_ERROR_CODE(err3, ErrorCode::UnexpectedToken);

        parser.free();
    }
}

TEST(IrTests, IrCompilerTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "dependency_test.mr");

    int dependencyCount = unit->getDependencyCount();
    EXPECT_EQ(dependencyCount, 1);

    // Simple sanity check to make sure it's the right file
    IrCompilationUnit *dep = unit->getDependency(0);
    EXPECT_EQ(dep->getNodeDefinitionCount(), 1);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrDependencyTreeTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "dependency_tree.mr");

    EXPECT_NE(unit, nullptr);
    const ErrorList *errors = compiler.getErrorList();
    EXPECT_EQ(errors->getErrorCount(), 0);

    int dependencyCount = unit->getDependencyCount();
    EXPECT_EQ(dependencyCount, 2);

    // Simple sanity check to make sure it's the right file
    IrCompilationUnit *dep = unit->getDependency(0);
    EXPECT_EQ(dep->getNodeDefinitionCount(), 1);

    IrCompilationUnit *dep2 = unit->getDependency(1);
    EXPECT_EQ(dep2->getNodeDefinitionCount(), 2);

    int secondaryDependencyCount = dep2->getDependencyCount();
    EXPECT_EQ(secondaryDependencyCount, 1);

    IrCompilationUnit *secondaryDep = dep2->getDependency(0);

    // Make sure that the compiler doesn't build a file twice
    EXPECT_EQ(dep, secondaryDep);
    EXPECT_EQ(compiler.getUnitCount(), 3);

    // Make sure the definitions were found
    EXPECT_TRUE(unit->getNode(0)->getDefinition() != nullptr);
    EXPECT_TRUE(unit->getNode(1)->getDefinition() != nullptr);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrMissingNodeDefinitionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "single_empty_node.mr");

    const ErrorList *errors = compiler.getErrorList();
    int errorCount = errors->getErrorCount();

    CompilationError *err = errors->getCompilationError(0);
    EXPECT_ERROR_CODE(err, ErrorCode::UndefinedNodeType);

    // Check that the location matches
    EXPECT_EQ(err->getErrorLocation()->lineStart, 1);
    EXPECT_EQ(err->getErrorLocation()->lineEnd, 1);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrAttributeDefinitionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "attribute_definition_test.mr");

    const ErrorList *errors = compiler.getErrorList();
    int errorCount = errors->getErrorCount();

    EXPECT_EQ(errorCount, 2);

    CompilationError *err0 = errors->getCompilationError(0);
    CompilationError *err1 = errors->getCompilationError(1);
    EXPECT_ERROR_CODE(err0, ErrorCode::UsingOutputPortAsInput);
    EXPECT_ERROR_CODE(err1, ErrorCode::PortNotFound);

    IrNodeDefinition *definition = unit->getNodeDefinition(0);
    IrNode *nodeInstance = unit->getNode(0);

    EXPECT_EQ(nodeInstance->getDefinition(), definition);
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(0)->getAttributeDefinition(), definition->getAttributeDefinition("A"));
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(1)->getAttributeDefinition(), definition->getAttributeDefinition("B"));
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(2)->getAttributeDefinition(), nullptr);
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(3)->getAttributeDefinition(), nullptr);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrPositionAttributeTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "position_attribute_test.mr");

    const ErrorList *errors = compiler.getErrorList();
    int errorCount = errors->getErrorCount();

    // 2 are from the dependency
    EXPECT_EQ(errorCount, 3);

    CompilationError *err2 = errors->getCompilationError(2);
    EXPECT_ERROR_CODE(err2, ErrorCode::ArgumentPositionOutOfBounds);

    IrNode *nodeInstance = unit->getNode(0);

    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(0)->getAttributeDefinition()->getName(), "A");
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(1)->getAttributeDefinition()->getName(), "B");
    EXPECT_EQ(nodeInstance->getAttributes()->getAttribute(2)->getAttributeDefinition(), nullptr);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrNodeBodyTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "node_body.mr");

    const ErrorList *errors = compiler.getErrorList();
    int errorCount = errors->getErrorCount();

    // No errors expected
    EXPECT_EQ(errorCount, 0);

    IrNode *nodeInstance = unit->getNode(0);
    IrNodeDefinition *definition = nodeInstance->getDefinition();
    IrNode *bodyNode = definition->getBody()->getItem(0);

    EXPECT_EQ(bodyNode->getName(), "add_mod");
    EXPECT_EQ(bodyNode->getType(), "Add");

    // Check that the node was resolved properly
    IrNodeDefinition *bodyDefinition = bodyNode->getDefinition();
    EXPECT_NE(bodyDefinition, nullptr);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrMissingDependencyTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "missing_dependency.mr");

    const ErrorList *errors = compiler.getErrorList();
    int errorCount = errors->getErrorCount();

    // Expect an import error
    EXPECT_EQ(errorCount, 1);

    CompilationError *err0 = errors->getCompilationError(0);
    EXPECT_ERROR_CODE(err0, ErrorCode::FileOpenFailed);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrReferenceResolutionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "reference_resolution.mr");

    const ErrorList *errors = compiler.getErrorList();
    const int errorCount = errors->getErrorCount();

    // Expect no errors
    EXPECT_EQ(errorCount, 0);

    IrNode *node = unit->getNode(0);
    IrParserStructure *b = node->resolveLocalName("B");
    EXPECT_TRUE(b->allowsExternalAccess());

    IrParserStructure::IrReferenceInfo info;
    IrParserStructure::IrReferenceQuery query;
    query.inputContext = new IrContextTree(nullptr);

    IrParserStructure *definition = b->getReference(query, &info);
    EXPECT_EQ(definition, nullptr);
    EXPECT_TRUE(info.reachedDeadEnd);

    IrNode *childNode = (IrNode *)node->resolveLocalName("C")->getReference(query);
    EXPECT_EQ(childNode->getType(), "ChildNode");
    EXPECT_EQ(childNode->getName(), "childNode");

    query.inputContext->free();

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrReferenceResolutionError1Test) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "resolution-tests/resolution_errors_1.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::AccessingInternalMember, 21));
    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 22));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 24));

    EXPECT_TRUE(findError(errors, ErrorCode::InputSpecifiedMultipleTimes, 32));
    EXPECT_TRUE(findError(errors, ErrorCode::InputSpecifiedMultipleTimes, 33));

    EXPECT_EQ(errors->getErrorCount(), 5);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrReferenceResolutionError2Test) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "resolution-tests/resolution_errors_2.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 5));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 7));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 7));

    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 12));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 13));

    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 25));

    EXPECT_TRUE(findError(errors, ErrorCode::InputNotConnected, 28));
    EXPECT_TRUE(findError(errors, ErrorCode::OutputWithNoDefinition, 31));

    EXPECT_TRUE(findError(errors, ErrorCode::BuiltinOutputWithDefinition, 43));

    EXPECT_TRUE(findError(errors, ErrorCode::InputSpecifiedMultipleTimesPositional, 47));
    EXPECT_TRUE(findError(errors, ErrorCode::InputSpecifiedMultipleTimes, 48));

    EXPECT_EQ(errors->getErrorCount(), 11);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrFullErrorTest1) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "full-error-testing/test_case_1.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UnidentifiedToken, 14));
    EXPECT_TRUE(findError(errors, ErrorCode::PortNotFound, 23));
    EXPECT_TRUE(findError(errors, ErrorCode::PortNotFound, 24));
    EXPECT_TRUE(findError(errors, ErrorCode::InputNotConnected, 22)); // x2

    EXPECT_EQ(errors->getErrorCount(), 5);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrFullErrorTest2) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "full-error-testing/test_case_2.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();
    
    EXPECT_TRUE(findError(errors, ErrorCode::UnidentifiedToken, 16));
    EXPECT_TRUE(findError(errors, ErrorCode::PortNotFound, 24));
    EXPECT_TRUE(findError(errors, ErrorCode::PortNotFound, 25));

    EXPECT_EQ(errors->getErrorCount(), 3);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrFullErrorTest3) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "full-error-testing/test_case_3.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UnidentifiedToken, 5));
    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 5));
    EXPECT_TRUE(findError(errors, ErrorCode::UnidentifiedToken, 13));
    EXPECT_TRUE(findError(errors, ErrorCode::InputNotConnected, 13));
    EXPECT_TRUE(findError(errors, ErrorCode::UnidentifiedToken, 14));
    EXPECT_TRUE(findError(errors, ErrorCode::InputNotConnected, 14));
    EXPECT_TRUE(findError(errors, ErrorCode::UnexpectedToken, 16));
    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 36));

    EXPECT_EQ(errors->getErrorCount(), 8);

    compiler.free();
}

TEST(IrTests, IrInputConnectionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "input_connection_test.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 23, nullptr, true));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 25, nullptr, true));

    EXPECT_EQ(errors->getErrorCount(), 2);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrOperationDefinitionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "operation_definition.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    IrNode *node = unit->getNode(0);
    EXPECT_EQ(node->getType(), "operator+");

    IrNodeDefinition *definition = unit->getNodeDefinition(0);
    EXPECT_EQ(definition->getName(), "operator+");

    // Expect no errors
    EXPECT_EQ(errors->getErrorCount(), 0);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrVisibilityTest1) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "/visibility-tests/visibility_test_1.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 3));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 4));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 5));

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 8));

    EXPECT_EQ(errors->getErrorCount(), 4);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrVisibilityTest2) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "/visibility-tests/visibility_test_2.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 12));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 14));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 16));

    EXPECT_EQ(errors->getErrorCount(), 3);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrDuplicateNodeDefinitionTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "duplicate_node_definition.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::DuplicateNodeDefinition, 1));
    EXPECT_TRUE(findError(errors, ErrorCode::DuplicateNodeDefinition, 6));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 7));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 8));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 9));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 10));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 11));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 12));
    EXPECT_TRUE(findError(errors, ErrorCode::OutputWithNoDefinition, 11));
    EXPECT_TRUE(findError(errors, ErrorCode::OutputWithNoDefinition, 12));
    EXPECT_TRUE(findError(errors, ErrorCode::SymbolUsedMultipleTimes, 14));

    EXPECT_EQ(errors->getErrorCount(), 11);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrGlobalNodeReferenceTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "global_reference_test.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 8));

    EXPECT_EQ(errors->getErrorCount(), 1);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrInfiniteLoopTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "infinite_loop_test_1.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::CircularReference, 2));
    EXPECT_TRUE(findError(errors, ErrorCode::CircularReference, 2, nullptr, true));

    EXPECT_EQ(errors->getErrorCount(), 2);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrInfiniteLoopTest2) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "infinite_loop_test_2.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::CircularReference, 16));

    EXPECT_EQ(errors->getErrorCount(), 1);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrInfiniteLoopTest3) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "infinite_loop_test_3.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::CircularDefinition, 2));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 9));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 10));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 11));

    EXPECT_EQ(errors->getErrorCount(), 4);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrInfiniteLoopTest4) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "infinite_loop_test_4.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::CircularDefinition, 2, nullptr, true));
    EXPECT_TRUE(findError(errors, ErrorCode::CircularDefinition, 6, nullptr, true));

    EXPECT_EQ(errors->getErrorCount(), 2);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrTests, IrBadTypeEnforcementTest) {
    MemoryTracker::get()->reset();

    Compiler compiler;
    IrCompilationUnit *unit = compiler.compile(IR_TEST_FILES "bad_type_enforcement.mr");
    EXPECT_NE(unit, nullptr);

    const ErrorList *errors = compiler.getErrorList();

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedNodeType, 2));

    EXPECT_EQ(errors->getErrorCount(), 1);

    compiler.free();
    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
