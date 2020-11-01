#include "../include/compiler.h"

#include "../include/ir_compilation_unit.h"
#include "../include/ir_import_statement.h"
#include "../include/compilation_error.h"
#include "../include/language_rules.h"
#include "../include/memory_tracker.h"

piranha::Compiler::Compiler(const LanguageRules *rules) {
    m_rules = rules;
    m_extension = ".mr";
}

piranha::Compiler::~Compiler() {
    /* void */
}

piranha::IrCompilationUnit *piranha::Compiler::analyze(const IrPath &scriptPath) {
    IrCompilationUnit *newUnit = getUnit(scriptPath);
    Path rootDir;
    scriptPath.getParentPath(&rootDir);

    if (newUnit == nullptr) {
        newUnit = TRACK(new IrCompilationUnit());
        newUnit->setErrorList(&m_errorList);
        IrCompilationUnit::ParseResult parseResult = newUnit->parseFile(scriptPath);

        if (parseResult == IrCompilationUnit::ParseResult::IoError) {
            delete TRACK(newUnit);
            return nullptr;
        }

        newUnit->setRules(m_rules);
        m_units.push_back(newUnit);

        const int importCount = newUnit->getImportStatementCount();
        for (int i = 0; i < importCount; i++) {
            IrImportStatement *s = newUnit->getImportStatement(i);
            std::string libName = s->getLibName();

            if (!hasEnding(libName, m_extension)) {
                libName += m_extension;
            }

            Path importPath(libName);
            Path fullImportPath = importPath.isAbsolute() 
                ? importPath // TODO: Warn about use of absolute path
                : rootDir.append(importPath);

            if (!fullImportPath.exists()) {
                Path resolvedPath;
                bool fileFound = resolvePath(importPath, &resolvedPath);

                if (!fileFound) {
                    newUnit->addCompilationError(TRACK(new CompilationError(*s->getSummaryToken(),
                        ErrorCode::FileOpenFailed)));
                    continue;
                }
                else fullImportPath = resolvedPath;
            }

            // Recursively build
            IrCompilationUnit *importUnit = analyze(fullImportPath);
            s->setUnit(importUnit);
            if (importUnit == nullptr) {
                newUnit->addCompilationError(TRACK(new CompilationError(*s->getSummaryToken(),
                    ErrorCode::FileOpenFailed)));
            }
            else newUnit->addDependency(importUnit);
        }
    }

    return newUnit;
}

piranha::IrCompilationUnit *piranha::Compiler::compile(const IrPath &scriptPath) {
    IrCompilationUnit *topLevel = analyze(scriptPath);

    // Resolution step
    resolve();

    // Validation step
    validate();

    return topLevel;
}

void piranha::Compiler::free() {
    for (IrCompilationUnit *unit : m_units) {
        unit->free();

        delete FTRACK(unit);
    }

    m_units.clear();

    m_errorList.free();
}

piranha::IrCompilationUnit *piranha::Compiler::getUnit(const IrPath &scriptPath) const {
    int nUnits = (int)m_units.size();

    for (int i = 0; i < nUnits; i++) {
        IrCompilationUnit *unit = m_units[i];
        if (unit->getPath() == scriptPath) {
            return unit;
        }
    }

    return nullptr;
}

void piranha::Compiler::addSearchPath(const IrPath &path) {
    m_searchPaths.push_back(path);
}

bool piranha::Compiler::resolvePath(const IrPath &path, IrPath *target) const {
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

bool piranha::Compiler::isPathEquivalent(const IrPath &a, const IrPath &b) const {
    return (a == b);
}

bool piranha::Compiler::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        int result = 
            fullString.compare(
                fullString.length() - ending.length(),
                ending.length(),
                ending);

        return result == 0;
    }
    else return false;
}

void piranha::Compiler::resolve() {
    int unitCount = getUnitCount();
    for (int i = 0; i < unitCount; i++) {
        IrCompilationUnit *unit = m_units[i];
        unit->resolveAll();
    }
}

void piranha::Compiler::validate() {
    int unitCount = getUnitCount();
    for (int i = 0; i < unitCount; i++) {
        IrCompilationUnit *unit = m_units[i];
        unit->validateAll();
    }
}
