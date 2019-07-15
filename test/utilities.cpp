#include <pch.h>

#include "utilities.h"

#include "../include/compilation_error.h"
#include "../include/error_list.h"

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
