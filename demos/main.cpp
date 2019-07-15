#include "reference_language_rules.h"

#include "../include/compiler.h"
#include "../include/node_program.h"
#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"

#include <iostream>

int main() {
	piranha::NodeProgram nodeProgram;
	piranha_demo::ReferenceLanguageRules languageRules;
	languageRules.setNodeProgram(&nodeProgram);
	nodeProgram.setRules(&languageRules);

	piranha::Compiler compiler;
	piranha::IrCompilationUnit *unit = compiler.compile("../../demos/piranha/number_adder.mr");
	unit->build(&nodeProgram);

	nodeProgram.execute();

	std::string pause;
	std::cin >> pause;

	return 0;
}
