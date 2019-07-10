#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <pch.h>

#include "error_list.h"
#include "compilation_error.h"

#include <string>

using namespace piranha;

#define CMF_PATH "../../../demos/cmfs/"
#define WORKSPACE_PATH "../../../workspace/"
#define TMP_PATH (WORKSPACE_PATH "tmp/")
#define IR_TEST_FILES "../../../test/sdl/"

#define CHECK_IR_POS(parserStructure, _colStart, _colEnd, _lineStart, _lineEnd)	\
	EXPECT_EQ((parserStructure)->getSummaryToken()->colStart,	(_colStart));		\
	EXPECT_EQ((parserStructure)->getSummaryToken()->colEnd,		(_colEnd));			\
	EXPECT_EQ((parserStructure)->getSummaryToken()->lineStart,	(_lineStart));		\
	EXPECT_EQ((parserStructure)->getSummaryToken()->lineEnd,	(_lineEnd));

#define EXPECT_ERROR_CODE(error, code_)						\
	EXPECT_EQ((error)->getErrorCode().stage, code_.stage);	\
	EXPECT_EQ((error)->getErrorCode().code, code_.code);

#define EXPECT_ERROR_CODE_LINE(error, code_, line)			\
	EXPECT_EQ((error)->getErrorCode().stage, code_.stage);	\
	EXPECT_EQ((error)->getErrorCode().code, code_.code);	\
	EXPECT_EQ((error)->getErrorLocation()->lineStart, line);

bool findError(const ErrorList *errorList, const ErrorCode_struct &errorCode,
	int line = -1, const IrCompilationUnit *unit = nullptr, bool instantiationError = false);

#endif /* TEST_UTILITIES_H */
