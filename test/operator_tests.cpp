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
#include "../include/fundamental_output.h"

#include "test_rules.h"
#include "utilities.h"

using namespace piranha;

TEST(IrOperatorTests, IrOperatorSanityCheck) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_sanity_check.mr", nullptr, &rules, &compiler);
    IrNode *node = unit->getNode(0);

    IrParserStructure::IrReferenceQuery query;
    query.inputContext = new IrContextTree(nullptr);
    query.recordErrors = false;
    const piranha::ChannelType *channelType = node
        ->getDefinition()
        ->getAttributeDefinitionList()
        ->getAliasOutput()
        ->getReference(query)
        ->getImmediateChannelType();
    query.inputContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest1) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_1.mr", nullptr, &rules, &compiler);
    IrNode *node = unit->getNode(0);

    IrParserStructure::IrReferenceQuery query;
    query.inputContext = new IrContextTree(nullptr);
    query.recordErrors = false;
    const piranha::ChannelType *channelType = node
        ->getDefinition()
        ->getAttributeDefinitionList()
        ->getAliasOutput()
        ->getReference(query)
        ->getImmediateChannelType();
    query.inputContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest2) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_2.mr", nullptr, &rules, &compiler);
    IrNode *node = unit->getNode(0);

    const IrAttributeDefinitionList *list = node
        ->getDefinition()
        ->getAttributeDefinitionList();

    IrContextTree *parentContext = new IrContextTree(nullptr);;

    const piranha::ChannelType *channelType;
    IrParserStructure::IrReferenceQuery query;
    query.inputContext = parentContext;
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parentContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    channelType = list
        ->getOutputDefinition("test2")
        ->getReference(query)
        ->getImmediateChannelType();
    parentContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    channelType = list
        ->getOutputDefinition("test3")
        ->getReference(query)
        ->getImmediateChannelType();
    parentContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    // Test with the alternate context
    query.inputContext = parentContext->newChild(node);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    channelType = list
        ->getOutputDefinition("test2")
        ->getReference(query)
        ->getImmediateChannelType();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    channelType = list
        ->getOutputDefinition("test3")
        ->getReference(query)
        ->getImmediateChannelType();
    parentContext->free();

    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest3) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_3.mr", nullptr, &rules, &compiler);
    IrNode *node1 = unit->getNode(0);
    IrNode *node2 = unit->getNode(1);
    IrNode *node3 = unit->getNode(2);

    const IrAttributeDefinitionList *list = node1
        ->getDefinition()
        ->getAttributeDefinitionList();

    const piranha::ChannelType *channelType;

    IrContextTree *parent = new IrContextTree(nullptr);
    IrParserStructure::IrReferenceQuery query;

    // Default context first
    query.inputContext = parent;
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::IntType);

    query.inputContext = parent->newChild(node1);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::IntType);

    query.inputContext = parent->newChild(node2);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::IntType);

    query.inputContext = parent->newChild(node3);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest4) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_4.mr", nullptr, &rules, &compiler);
    IrNode *node1 = unit->getNode(0);
    IrNode *node2 = unit->getNode(1);
    IrNode *node3 = unit->getNode(2);

    const IrAttributeDefinitionList *list = node1
        ->getDefinition()
        ->getAttributeDefinitionList();

    const piranha::ChannelType *channelType;

    IrContextTree *parent = new IrContextTree(nullptr);
    IrParserStructure::IrReferenceQuery query;

    query.inputContext = parent->newChild(node1);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    query.inputContext = parent->newChild(node2);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::IntType);

    query.inputContext = parent->newChild(node3);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest5) {
    MemoryTracker::get()->reset();

    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_5.mr", nullptr, &rules, &compiler);
    IrNode *node1 = unit->getNode(0);
    IrNode *node2 = unit->getNode(1);

    const IrAttributeDefinitionList *list = node1
        ->getDefinition()
        ->getAttributeDefinitionList();

    const piranha::ChannelType *channelType;

    IrContextTree *parent = new IrContextTree(nullptr);
    IrParserStructure::IrReferenceQuery query;

    query.inputContext = parent->newChild(node1);
    query.recordErrors = false;
    channelType = list
        ->getOutputDefinition("test1")
        ->getReference(query)
        ->getImmediateChannelType();
    parent->free();
    EXPECT_EQ(channelType, &FundamentalType::FloatType);

    compiler->free();

    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest6) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_6.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 9, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 5, nullptr, true));

    EXPECT_EQ(errList->getErrorCount(), 2);

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest7) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_7.mr", &errList, nullptr, &compiler);

    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 9, nullptr, false));
    EXPECT_TRUE(findError(errList, ErrorCode::InvalidOperandTypes, 5, nullptr, true));

    EXPECT_EQ(errList->getErrorCount(), 2);

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest8) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    LanguageRules *rules;
    Compiler *compiler;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_8.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.execute();

    std::string result;
    program.getTopLevelContainer()->getNode(2)->getPrimaryOutput()->fullCompute((void *)&result);
    EXPECT_EQ(result, "Hello world!");

    program.getTopLevelContainer()->getNode(5)->getPrimaryOutput()->fullCompute((void *)&result);
    EXPECT_EQ(result, "Goodbye world!");

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrOperatorTests, IrOperatorTest9) {
    MemoryTracker::get()->reset();

    const ErrorList *errList;
    Compiler *compiler;
    LanguageRules *rules;
    IrCompilationUnit *unit = compileToUnit("operator-tests/operator_test_9.mr", &errList, &rules, &compiler);

    EXPECT_EQ(errList->getErrorCount(), 0);

    NodeProgram program;
    unit->build(&program);

    program.free();
    compiler->free();
    delete rules;

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
