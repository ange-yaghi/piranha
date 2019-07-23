#include "reference_language_rules.h"
#include "utilities.h"

#include "../include/compiler.h"
#include "../include/node_program.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"

#include <iostream>
#include <string>
#include <chrono>

int main() {
    std::string filename;
    std::cin >> filename;

    while (true) {
		auto startCompile = std::chrono::high_resolution_clock::now();

        std::cout << " -- Compiling --------------" << std::endl;
        piranha::NodeProgram nodeProgram;
        piranha_demo::ReferenceLanguageRules languageRules;
        languageRules.registerBuiltinNodeTypes();
        nodeProgram.setRules(&languageRules);

        piranha::Compiler compiler(&languageRules);
        piranha::IrCompilationUnit *unit = compiler.compile(filename);

        piranha_demo::printErrorTrace(compiler.getErrorList());

        if (compiler.getErrorList()->getErrorCount() == 0) {
			unit->build(&nodeProgram);

			auto endCompile = std::chrono::high_resolution_clock::now();

			std::cout << " -- Running --------------" << std::endl;
            nodeProgram.execute();

			auto endExecute = std::chrono::high_resolution_clock::now();

			nodeProgram.writeAssembly("asm.txt");

			std::cout << std::endl;
			std::cout << " ----------------------" << std::endl;
			std::cout << " Compile time:   " << 
				(endCompile - startCompile).count() * 1.0e-6 << " ms" << std::endl;
			std::cout << " Execution time: " << 
				(endExecute - endCompile).count() * 1.0e-6 << " ms" << std::endl;
        }
        std::cout << std::endl << std::endl;
        std::cin.get();
    }

    return 0;
}
