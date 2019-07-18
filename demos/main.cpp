#include "reference_language_rules.h"
#include "utilities.h"

#include "../include/compiler.h"
#include "../include/node_program.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"

#include <iostream>
#include <string>

int main() {
	std::string filename;
	std::cin >> filename;

	while (true) {
		std::cout << " -- Compiling and running --------------" << std::endl;
		piranha::NodeProgram nodeProgram;
		piranha_demo::ReferenceLanguageRules languageRules;
		languageRules.setNodeProgram(&nodeProgram);
		languageRules.registerBuiltinNodeTypes();
		nodeProgram.setRules(&languageRules);

		piranha::Compiler compiler(&languageRules);
		piranha::IrCompilationUnit *unit = compiler.compile(filename);

		piranha_demo::printErrorTrace(compiler.getErrorList());

		if (compiler.getErrorList()->getErrorCount() == 0) {
			unit->build(&nodeProgram);
			nodeProgram.execute();
		}

		std::cout << std::endl << std::endl;
		std::cin.get();
	}

	return 0;
}
