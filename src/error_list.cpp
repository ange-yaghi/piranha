#include "../include/error_list.h"

#include "../include/compilation_error.h"

piranha::ErrorList::ErrorList() {
    /* void */
}

piranha::ErrorList::~ErrorList() {
    /* void */
}

void piranha::ErrorList::addCompilationError(CompilationError *err) {
    m_compilationErrors.push_back(err);
}

void piranha::ErrorList::destroy() {
    int errorCount = getErrorCount();

    for (int i = 0; i < errorCount; i++) {
        delete m_compilationErrors[i];
    }

    m_compilationErrors.clear();
}
