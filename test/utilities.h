#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include "../include/error_list.h"
#include "../include/compilation_error.h"
#include "../include/compiler.h"

#include <string>

using namespace piranha;

#define CMF_PATH "../../demos/cmfs/"
#define WORKSPACE_PATH "../../workspace/"
#define TMP_PATH (WORKSPACE_PATH "tmp/")
#define IR_TEST_FILES "../../test/sdl/"

#define CHECK_IR_POS(IrParserStructure, _colStart, _colEnd, _lineStart, _lineEnd)    \
    EXPECT_EQ((IrParserStructure)->getSummaryToken()->colStart,    (_colStart));        \
    EXPECT_EQ((IrParserStructure)->getSummaryToken()->colEnd,        (_colEnd));            \
    EXPECT_EQ((IrParserStructure)->getSummaryToken()->lineStart,    (_lineStart));        \
    EXPECT_EQ((IrParserStructure)->getSummaryToken()->lineEnd,    (_lineEnd));

#define EXPECT_ERROR_CODE(error, code_)                        \
    EXPECT_EQ((error)->getErrorCode().stage, code_.stage);    \
    EXPECT_EQ((error)->getErrorCode().code, code_.code);

#define EXPECT_ERROR_CODE_LINE(error, code_, line)            \
    EXPECT_EQ((error)->getErrorCode().stage, code_.stage);    \
    EXPECT_EQ((error)->getErrorCode().code, code_.code);    \
    EXPECT_EQ((error)->getErrorLocation()->lineStart, line);

bool findError(const ErrorList *errorList, const ErrorCode_struct &errorCode,
    int line = -1, const IrCompilationUnit *unit = nullptr, bool instantiationError = false);

IrCompilationUnit *compileFile(
    const std::string &filename, const ErrorList **errList = nullptr, 
    LanguageRules **outputRules = nullptr, Compiler **compiler = nullptr);
IrCompilationUnit *compileToUnit(
    const std::string &filename, const ErrorList **errList = nullptr, 
    LanguageRules **outputRules = nullptr, Compiler **compiler = nullptr);

#endif /* TEST_UTILITIES_H */
