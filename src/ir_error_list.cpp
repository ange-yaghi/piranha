#include "ir_error_list.h"

#include "ir_compilation_error.h"

piranha::IrErrorList::IrErrorList() {
	/* void */
}

piranha::IrErrorList::~IrErrorList() {
	/* void */
}

void piranha::IrErrorList::addCompilationError(IrCompilationError *err) {
	m_compilationErrors.push_back(err);
}

void piranha::IrErrorList::destroy() {
	int errorCount = getErrorCount();

	for (int i = 0; i < errorCount; i++) {
		delete m_compilationErrors[i];
	}

	m_compilationErrors.clear();
}
