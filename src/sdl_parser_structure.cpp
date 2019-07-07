#include <sdl_parser_structure.h>

#include <sdl_compilation_unit.h>
#include <sdl_context_tree.h>

piranha::SdlParserStructure::SdlReferenceInfo::SdlReferenceInfo() {
	newContext = nullptr;
	err = nullptr;

	failed = false;
	reachedDeadEnd = false;
	touchedMainContext = false;
}

piranha::SdlParserStructure::SdlReferenceInfo::~SdlReferenceInfo() {
	/* void */
}

void piranha::SdlParserStructure::SdlReferenceInfo::reset() {
	newContext = nullptr;
	err = nullptr;

	failed = false;
	reachedDeadEnd = false;
	touchedMainContext = false;
}

piranha::SdlParserStructure::SdlReferenceQuery::SdlReferenceQuery() {
	inputContext = nullptr;
	recordErrors = false;
}

piranha::SdlParserStructure::SdlReferenceQuery::~SdlReferenceQuery() {
	/* void */
}


piranha::SdlParserStructure::SdlParserStructure() {
	m_parentScope = nullptr;
	m_logicalParent = nullptr;
	m_checkReferences = true;

	m_definitionsResolved = false;
	m_validated = false;

	m_visibility = SdlVisibility::DEFAULT;
	m_defaultVisibility = SdlVisibility::PRIVATE;
}

piranha::SdlParserStructure::~SdlParserStructure() {
	/* void */
}

void piranha::SdlParserStructure::registerToken(const SdlTokenInfo *tokenInfo) {
	if (tokenInfo != nullptr) m_summaryToken.combine(tokenInfo);
}

void piranha::SdlParserStructure::registerComponent(SdlParserStructure *child) {
	if (child != nullptr) {
		m_summaryToken.combine(child->getSummaryToken());
		child->setParentScope(this);
		child->setLogicalParent(this);

		m_components.push_back(child);
	}
}

piranha::SdlParserStructure *piranha::SdlParserStructure::resolveName(const std::string &name) const {
	SdlParserStructure *local = resolveLocalName(name);
	if (local != nullptr) return local;
	
	if (m_parentScope != nullptr) {
		return m_parentScope->resolveName(name);
	}

	return nullptr;
}

piranha::SdlParserStructure *piranha::SdlParserStructure::getImmediateReference(const SdlReferenceQuery &query, SdlReferenceInfo *output) {
	return nullptr;
}

piranha::SdlParserStructure *piranha::SdlParserStructure::getReference(const SdlReferenceQuery &query, SdlReferenceInfo *output) {
	SDL_RESET(query);

	SdlReferenceQuery immediateQuery = query;
	SdlReferenceInfo immediateInfo;
	SdlParserStructure *immediateReference = getImmediateReference(immediateQuery, &immediateInfo);

	if (immediateInfo.failed) {
		SDL_FAIL();
		SDL_ERR_OUT(immediateInfo.err);
		return nullptr;
	}

	if (immediateInfo.reachedDeadEnd) {
		SDL_DEAD_END();
		return nullptr;
	}

	if (immediateReference != nullptr) {
		SdlReferenceInfo nestedInfo;
		SdlReferenceQuery nestedQuery;
		nestedQuery.inputContext = immediateInfo.newContext;

		// Error checking is not done on any parent nodes because it's assumed that errors have
		// already been checked/reported
		nestedQuery.recordErrors = false;

		SdlParserStructure *fullReference = immediateReference->getReference(nestedQuery, &nestedInfo);

		if (nestedInfo.failed) {
			SDL_FAIL();
			SDL_ERR_OUT(nestedInfo.err);
			return nullptr;
		}

		if (nestedInfo.reachedDeadEnd) {
			SDL_DEAD_END();
			return nullptr;
		}

		SDL_INFO_OUT(newContext, nestedInfo.newContext);
		SDL_INFO_OUT(touchedMainContext, nestedInfo.touchedMainContext || immediateInfo.touchedMainContext);
		return fullReference;
	}
	else {
		return this;
	}
}

void piranha::SdlParserStructure::resolveDefinitions() {
	if (m_definitionsResolved) return;

	// Resolve components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->resolveDefinitions();
	}

	_resolveDefinitions();

	m_definitionsResolved = true;
}

void piranha::SdlParserStructure::checkReferences(SdlContextTree *inputContext) {
	// Check components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->checkReferences(inputContext);
	}

	if (m_checkReferences) {
		SdlReferenceQuery query;
		query.inputContext = inputContext;
		query.recordErrors = true;
		SdlReferenceInfo info;

		SdlParserStructure *reference = getReference(query, &info);

		if (info.err != nullptr) {
			getParentUnit()->addCompilationError(info.err);
		}
	}
}

void piranha::SdlParserStructure::checkInstantiation() {
	// Check components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->checkInstantiation();
	}

	_checkInstantiation();
}

void piranha::SdlParserStructure::validate() {
	if (m_validated) return;

	// Validate components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->validate();
	}

	_validate();

	m_validated = true;
}

void piranha::SdlParserStructure::expand() {
	if (m_isExpanded) return;

	// Expand components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->expand();
	}

	_expand();

	if (m_expansion != nullptr) {
		// If this structure is found to result in an expanded form,
		// then it will also be assumed that it references that expansion
		//m_reference = m_expansion;
		//m_referencesResolved = true;

		registerComponent(m_expansion);
	}

	m_isExpanded = true;
}

void piranha::SdlParserStructure::_validate() {
	/* void */
}

void piranha::SdlParserStructure::_checkInstantiation() {
	/* void */
}

void piranha::SdlParserStructure::writeReferencesToFile(std::ofstream &file, SdlContextTree *context, int tabLevel) {
	for (int i = 0; i < tabLevel; i++) {
		file << " ";
	}

	if (m_summaryToken.lineStart != m_summaryToken.lineEnd) {
		file << "L[" << m_summaryToken.lineStart << "-" << m_summaryToken.lineEnd << "]";
	}
	else {
		file << "L[" << m_summaryToken.lineStart << "]";
	}

	file << "[" << m_summaryToken.colStart << "-" << m_summaryToken.colEnd << "]";

	SdlReferenceInfo info;
	SdlReferenceQuery query;
	query.inputContext = context;
	query.recordErrors = false;

	SdlNode *asNode = getAsNode();
	SdlParserStructure *immediateReference = getImmediateReference(query, &info);

	if (info.failed) {
		file << " => "; 
		file << "ERROR" << std::endl;
		return;
	}
	else if (immediateReference == nullptr) {
		file << std::endl;
		return;
	}
	else {
		file << " => ";
		immediateReference->writeReferencesToFile(file, info.newContext);
	}
}

void piranha::SdlParserStructure::_resolveDefinitions() {
	/* void */
}

void piranha::SdlParserStructure::_expand() {
	m_expansion = nullptr;
}

piranha::SdlParserStructure *piranha::SdlParserStructure::resolveLocalName(const std::string &name) const {
	return nullptr;
}

bool piranha::SdlParserStructure::allowsExternalAccess() const {
	SdlVisibility visibility = (m_visibility == SdlVisibility::DEFAULT) ? m_defaultVisibility : m_visibility;
	return visibility == SdlVisibility::PUBLIC;
}

piranha::SdlCompilationUnit *piranha::SdlParserStructure::getParentUnit() const {
	if (m_parentUnit == nullptr) return m_logicalParent->getParentUnit();
	else return m_parentUnit;
}
