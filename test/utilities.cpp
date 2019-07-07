#include <pch.h>

#include <utilities.h>

#include <sdl_compilation_error.h>
#include <sdl_error_list.h>

bool findError(const SdlErrorList *errorList, const SdlErrorCode_struct &errorCode, int line, 
										const SdlCompilationUnit *unit, bool instantiationError) {
	int errorCount = errorList->getErrorCount();

	for (int i = 0; i < errorCount; i++) {
		SdlCompilationError *error = errorList->getCompilationError(i);
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
