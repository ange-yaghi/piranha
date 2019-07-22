#include "utilities.h"

#include "../include/ir_compilation_unit.h"
#include "../include/path.h"
#include "../include/ir_context_tree.h"
#include "../include/ir_node.h"
#include "../include/ir_node_definition.h"

#include <iostream>

void piranha_demo::printError(const piranha::CompilationError *err) {
    const piranha::ErrorCode_struct &errorCode = err->getErrorCode();
    std::cout << err->getCompilationUnit()->getPath().getStem() << "(" << err->getErrorLocation()->lineStart << "): error " << errorCode.stage << errorCode.code << ": " << errorCode.info << std::endl;

    piranha::IrContextTree *context = err->getInstantiation();
    while (context != nullptr) {
        piranha::IrNode *instance = context->getContext();
        if (instance != nullptr) {
            std::string instanceName = instance->getName();
            std::string definitionName = (instance->getDefinition() != nullptr) ? instance->getDefinition()->getName() : std::string("<Type Error>");
            std::string formattedName;
            if (instanceName.empty()) formattedName = "<unnamed> " + definitionName;
            else formattedName = instanceName + " " + definitionName;

            std::cout <<
                "       While instantiating: " <<
                instance->getParentUnit()->getPath().getStem() << "(" << instance->getSummaryToken()->lineStart << "): " << formattedName << std::endl;
        }

        context = context->getParent();
    }
}

void piranha_demo::printErrorTrace(const piranha::ErrorList *errList) {
    int errorCount = errList->getErrorCount();
    for (int i = 0; i < errorCount; i++) {
        printError(errList->getCompilationError(i));
    }
}
