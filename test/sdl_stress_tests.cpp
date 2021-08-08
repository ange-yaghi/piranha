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

#include "utilities.h"

using namespace piranha;

TEST(IrSyntaxStressTests, IrSyntaxStressTest1) {
    MemoryTracker::get()->reset();

    const ErrorList *errors;
    Compiler *compiler;
    compileFile("stress-testing/stress_test_1.mr", &errors, nullptr, &compiler);

    EXPECT_EQ(errors->getErrorCount(), 10);

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedBuiltinType, 16));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedBuiltinType, 20));

    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 50));
    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 51));
    EXPECT_TRUE(findError(errors, ErrorCode::UnresolvedReference, 52));

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 55));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 56));

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 60));

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 62));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 67));

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrSyntaxStressTests, IrSyntaxNodeArgumentStressTest1) {
    MemoryTracker::get()->reset();
    
    const ErrorList *errors;
    Compiler *compiler;
    compileFile("stress-testing/node_argument_stress_test_1.mr", &errors, nullptr, &compiler);

    EXPECT_EQ(errors->getErrorCount(), 5);

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 22, nullptr, true));
    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 23, nullptr));

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrSyntaxStressTests, IrSyntaxDeepErrorIsolated) {
    MemoryTracker::get()->reset();

    const ErrorList *errors;
    Compiler *compiler;
    compileFile("stress-testing/deep_error_isolated.mr", &errors, nullptr, &compiler);

    EXPECT_EQ(errors->getErrorCount(), 1);

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 3, nullptr, true));

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}

TEST(IrSyntaxStressTests, IrSyntaxDeepError) {
    MemoryTracker::get()->reset();

    const ErrorList *errors;
    Compiler *compiler;
    compileFile("stress-testing/deep_error.mr", &errors, nullptr, &compiler);

    EXPECT_EQ(errors->getErrorCount(), 3);

    EXPECT_TRUE(findError(errors, ErrorCode::UndefinedMember, 27, nullptr, true));

    compiler->free();

    EXPECT_EQ(MemoryTracker::get()->countLeaks(), 0);
}
