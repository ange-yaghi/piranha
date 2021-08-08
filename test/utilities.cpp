#include <gtest/gtest.h>

#include "test_rules.h"
#include "utilities.h"

#include "../include/compilation_error.h"
#include "../include/error_list.h"
#include "../include/compiler.h"

bool findError(const ErrorList *errorList, const ErrorCode_struct &errorCode, int line, 
                                        const IrCompilationUnit *unit, bool instantiationError) {
    int errorCount = errorList->getErrorCount();

    for (int i = 0; i < errorCount; i++) {
        CompilationError *error = errorList->getCompilationError(i);
        if (unit == nullptr || error->getCompilationUnit() == unit) {
            if (error->getErrorCode().code == errorCode.code && error->getErrorCode().stage == errorCode.stage) {
                if (line == -1 || error->getErrorLocation()->lineStart == line) {
                    if (error->isInstantiationError() == instantiationError) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

IrCompilationUnit *compileFile(const std::string &filename, const ErrorList **errList, LanguageRules **outputRules, Compiler **_compiler) {
    TestRules *rules = new TestRules();
    rules->initialize();

    piranha::Compiler *compiler = new Compiler(rules);
    IrCompilationUnit *unit = compiler->compile(IR_TEST_FILES + filename);
    EXPECT_NE(unit, nullptr);

    if (errList != nullptr) *errList = compiler->getErrorList();
    if (_compiler != nullptr) *_compiler = compiler;
    if (outputRules != nullptr) *outputRules = rules;
    else delete rules;

    return unit;
}

IrCompilationUnit *compileToUnit(
    const std::string &filename, const ErrorList **errList, 
    LanguageRules **outputRules, Compiler **_compiler) 
{
    TestRules *rules = new TestRules();
    rules->initialize();

    Compiler *compiler = new Compiler(rules);

    IrCompilationUnit *unit = compiler->compile(IR_TEST_FILES + filename);
    EXPECT_NE(unit, nullptr);

    if (errList != nullptr) *errList = compiler->getErrorList();
    if (outputRules != nullptr) *outputRules = rules;
    else delete rules;

    if (_compiler != nullptr) *_compiler = compiler;
    else {
        compiler->free();
        delete compiler;
    }

    return unit;
}
