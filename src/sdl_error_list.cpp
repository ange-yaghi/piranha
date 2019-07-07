#include <sdl_error_list.h>

#include <sdl_compilation_error.h>

piranha::SdlErrorList::SdlErrorList() {
	/* void */
}

piranha::SdlErrorList::~SdlErrorList() {
	/* void */
}

void piranha::SdlErrorList::addCompilationError(SdlCompilationError *err) {
	m_compilationErrors.push_back(err);
}

void piranha::SdlErrorList::destroy() {
	int errorCount = getErrorCount();

	for (int i = 0; i < errorCount; i++) {
		delete m_compilationErrors[i];
	}

	m_compilationErrors.clear();
}
