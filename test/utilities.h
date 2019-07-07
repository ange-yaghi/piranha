#ifndef TEST_UTILITIES_H
#define TEST_UTILITIES_H

#include <pch.h>

#include <sdl_error_list.h>
#include <sdl_compilation_error.h>

#include <string>

using namespace piranha;

#define CMF_PATH "../../../demos/cmfs/"
#define WORKSPACE_PATH "../../../workspace/"
#define TMP_PATH (WORKSPACE_PATH "tmp/")
#define SDL_TEST_FILES "../../../test/sdl/"

#define CHECK_SDL_POS(parserStructure, _colStart, _colEnd, _lineStart, _lineEnd)	\
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

bool findError(const SdlErrorList *errorList, const SdlErrorCode_struct &errorCode,
	int line = -1, const SdlCompilationUnit *unit = nullptr, bool instantiationError = false);

#endif /* TEST_UTILITIES_H */
