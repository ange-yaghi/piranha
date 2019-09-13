#ifndef PIRANHA_REFERENCE_COMPILER_UTILITIES_H
#define PIRANHA_REFERENCE_COMPILER_UTILITIES_H

#include "../include/compilation_error.h"
#include "../include/error_list.h"

namespace piranha_demo {

    void printError(const piranha::CompilationError *err);
    void printErrorTrace(const piranha::ErrorList *errList);

} /* namespace piranha_demo */

#endif /* PIRANHA_REFERENCE_COMPILER_UTILITIES_H */
