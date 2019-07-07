#include <sdl_import_statement.h>

piranha::SdlImportStatement::SdlImportStatement(const SdlTokenInfo_string &libName) : m_libName(libName) {
	registerToken(&libName);
	m_unit = nullptr;
}

piranha::SdlImportStatement::~SdlImportStatement() {
	/* void */
}
