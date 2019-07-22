#include "../include/ir_import_statement.h"

piranha::IrImportStatement::IrImportStatement(const IrTokenInfo_string &libName) 
    : m_libName(libName)
{
    registerToken(&libName);
    m_unit = nullptr;
}

piranha::IrImportStatement::~IrImportStatement() {
    /* void */
}
