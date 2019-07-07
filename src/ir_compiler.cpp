#include "compiler.h"

#include "ir_compilation_unit.h"
#include "ir_import_statement.h"
#include "ir_compilation_error.h"

piranha::IrCompiler::IrCompiler() {
	/* void */
}

piranha::IrCompiler::~IrCompiler() {
	/* void */
}

piranha::IrCompilationUnit *piranha::IrCompiler::analyze(const IrPath &scriptPath) {
	IrCompilationUnit *newUnit = getUnit(scriptPath);
	Path rootDir;
	scriptPath.getParentPath(&rootDir);

	if (newUnit == nullptr) {
		newUnit = new IrCompilationUnit();
		newUnit->setErrorList(&m_errorList);
		IrCompilationUnit::ParseResult parseResult = newUnit->parseFile(scriptPath);

		if (parseResult == IrCompilationUnit::IO_ERROR) {
			delete newUnit;
			return nullptr;
		}

		m_units.push_back(newUnit);

		int importCount = newUnit->getImportStatementCount();
		for (int i = 0; i < importCount; i++) {
			IrImportStatement *s = newUnit->getImportStatement(i);
			std::string libName = s->getLibName();

			if (!hasEnding(libName, ".mr")) {
				libName += ".mr";
			}

			Path importPath(libName);
			Path fullImportPath;

			if (importPath.isAbsolute()) {
				// TODO: Warn about use of absolute path
				fullImportPath = importPath;
			}
			else {
				fullImportPath = rootDir.append(importPath);
			}

			if (!fullImportPath.exists()) {
				Path resolvedPath;
				bool fileFound = resolvePath(importPath, &resolvedPath);

				if (!fileFound) {
					newUnit->addCompilationError(new IrCompilationError(*s->getSummaryToken(),
						IrErrorCode::FileOpenFailed));
					continue;
				}
				else fullImportPath = resolvedPath;
			}

			// Recursively build
			IrCompilationUnit *importUnit = analyze(fullImportPath);
			s->setUnit(importUnit);
			if (importUnit == nullptr) {
				newUnit->addCompilationError(new IrCompilationError(*s->getSummaryToken(),
					IrErrorCode::FileOpenFailed));
			}
			else newUnit->addDependency(importUnit);
		}
	}

	return newUnit;
}

piranha::IrCompilationUnit *piranha::IrCompiler::compile(const IrPath &scriptPath) {
	IrCompilationUnit *topLevel = analyze(scriptPath);

	// Expansion step
	expand();

	// Resolution step
	resolve();

	// Validation step
	validate();

	return topLevel;
}

piranha::IrCompilationUnit *piranha::IrCompiler::getUnit(const IrPath &scriptPath) const {
	int nUnits = (int)m_units.size();

	for (int i = 0; i < nUnits; i++) {
		IrCompilationUnit *unit = m_units[i];
		if (unit->getPath() == scriptPath) {
			return unit;
		}
	}

	return nullptr;
}

void piranha::IrCompiler::addSearchPath(const IrPath &path) {
	m_searchPaths.push_back(path);
}

bool piranha::IrCompiler::resolvePath(const IrPath &path, IrPath *target) const {
	if (path.exists()) {
		*target = path;
		return true;
	}

	int searchPathCount = getSearchPathCount();
	for (int i = 0; i < searchPathCount; i++) {
		IrPath total = m_searchPaths[i].append(path);

		if (total.exists()) {
			*target = total;
			return true;
		}
	}

	return false;
}

bool piranha::IrCompiler::isPathEquivalent(const IrPath &a, const IrPath &b) const {
	return (a == b);
}

bool piranha::IrCompiler::hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else return false;
}

void piranha::IrCompiler::expand() {
	int unitCount = getUnitCount();
	for (int i = 0; i < unitCount; i++) {
		IrCompilationUnit *unit = m_units[i];
		unit->expand();
	}
}

void piranha::IrCompiler::resolve() {
	int unitCount = getUnitCount();
	for (int i = 0; i < unitCount; i++) {
		IrCompilationUnit *unit = m_units[i];
		unit->resolveDefinitions();
		unit->checkInstantiation();
	}
}

void piranha::IrCompiler::validate() {
	int unitCount = getUnitCount();
	for (int i = 0; i < unitCount; i++) {
		IrCompilationUnit *unit = m_units[i];
		unit->validate();
	}
}
