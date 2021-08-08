#include "reference_language_rules.h"
#include "utilities.h"

#include "../include/compiler.h"
#include "../include/node_program.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"
#include "../include/memory_tracker.h"

#include <iostream>
#include <ostream>
#include <string>
#include <chrono>

int main() {
    std::string filename;
    std::cout << "Enter .mr file: ";
    std::cin >> filename;

    while (true) {
        auto startCompile = std::chrono::high_resolution_clock::now();

        {
            std::cout << "--- Compiling --------------" << std::endl;
            piranha::NodeProgram nodeProgram;
            piranha_demo::ReferenceLanguageRules languageRules;
            languageRules.initialize();

            piranha::Compiler compiler(&languageRules);
            piranha::IrCompilationUnit *unit = compiler.compile(filename);

            piranha_demo::printErrorTrace(compiler.getErrorList());

            if (unit == nullptr) {
                std::cout << "Could not find file: " << filename << std::endl;
            }
            else if (compiler.getErrorList()->getErrorCount() == 0) {
                unit->build(&nodeProgram);

                std::cout << "--- Running ----------------" << std::endl;
                auto endCompile = std::chrono::high_resolution_clock::now();

                nodeProgram.initialize();
                //nodeProgram.optimize();
                nodeProgram.execute();

                auto endExecute = std::chrono::high_resolution_clock::now();

                nodeProgram.writeAssembly("../workspace/asm/" + unit->getPath().getStem() + ".pasm");

                std::cout << std::endl;
                std::cout << "----------------------------" << std::endl;
                std::cout << " Compile time:   " <<
                    (endCompile - startCompile).count() * 1.0e-6 << " ms" << std::endl;
                std::cout << " Execution time: " <<
                    (endExecute - endCompile).count() * 1.0e-6 << " ms" << std::endl;

                nodeProgram.free();
            }

            compiler.free();
        }

        std::cout << "Memory leaks: " << piranha::MemoryTracker::get()->countLeaks() << std::endl;
        piranha::MemoryTracker::get()->reset();

        std::cout << std::endl;
        std::cout << "Run again (y/n)? ";

        char cmd;
        std::cin >> cmd;

        if (cmd != 'y') break;

        std::cout << std::endl;
    }

    return 0;
}
