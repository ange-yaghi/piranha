#include "ir_parser_structure.h"

#include "ir_compilation_unit.h"
#include "ir_context_tree.h"

piranha::ParserStructure::IrReferenceInfo::IrReferenceInfo() {
	newContext = nullptr;
	err = nullptr;

	failed = false;
	reachedDeadEnd = false;
	touchedMainContext = false;
}

piranha::ParserStructure::IrReferenceInfo::~IrReferenceInfo() {
	/* void */
}

void piranha::ParserStructure::IrReferenceInfo::reset() {
	newContext = nullptr;
	err = nullptr;

	failed = false;
	reachedDeadEnd = false;
	touchedMainContext = false;
}

piranha::ParserStructure::IrReferenceQuery::IrReferenceQuery() {
	inputContext = nullptr;
	recordErrors = false;
}

piranha::ParserStructure::IrReferenceQuery::~IrReferenceQuery() {
	/* void */
}


piranha::ParserStructure::ParserStructure() {
	m_parentScope = nullptr;
	m_logicalParent = nullptr;
	m_checkReferences = true;

	m_definitionsResolved = false;
	m_validated = false;

	m_visibility = IrVisibility::DEFAULT;
	m_defaultVisibility = IrVisibility::PRIVATE;
}

piranha::ParserStructure::~ParserStructure() {
	/* void */
}

void piranha::ParserStructure::registerToken(const IrTokenInfo *tokenInfo) {
	if (tokenInfo != nullptr) m_summaryToken.combine(tokenInfo);
}

void piranha::ParserStructure::registerComponent(ParserStructure *child) {
	if (child != nullptr) {
		m_summaryToken.combine(child->getSummaryToken());
		child->setParentScope(this);
		child->setLogicalParent(this);

		m_components.push_back(child);
	}
}

piranha::ParserStructure *piranha::ParserStructure::resolveName(const std::string &name) const {
	ParserStructure *local = resolveLocalName(name);
	if (local != nullptr) return local;
	
	if (m_parentScope != nullptr) {
		return m_parentScope->resolveName(name);
	}

	return nullptr;
}

piranha::ParserStructure *piranha::ParserStructure::getImmediateReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	return nullptr;
}

piranha::ParserStructure *piranha::ParserStructure::getReference(const IrReferenceQuery &query, IrReferenceInfo *output) {
	IR_RESET(query);

	IrReferenceQuery immediateQuery = query;
	IrReferenceInfo immediateInfo;
	ParserStructure *immediateReference = getImmediateReference(immediateQuery, &immediateInfo);

	if (immediateInfo.failed) {
		IR_FAIL();
		IR_ERR_OUT(immediateInfo.err);
		return nullptr;
	}

	if (immediateInfo.reachedDeadEnd) {
		IR_DEAD_END();
		return nullptr;
	}

	if (immediateReference != nullptr) {
		IrReferenceInfo nestedInfo;
		IrReferenceQuery nestedQuery;
		nestedQuery.inputContext = immediateInfo.newContext;

		// Error checking is not done on any parent nodes because it's assumed that errors have
		// already been checked/reported
		nestedQuery.recordErrors = false;

		ParserStructure *fullReference = immediateReference->getReference(nestedQuery, &nestedInfo);

		if (nestedInfo.failed) {
			IR_FAIL();
			IR_ERR_OUT(nestedInfo.err);
			return nullptr;
		}

		if (nestedInfo.reachedDeadEnd) {
			IR_DEAD_END();
			return nullptr;
		}

		IR_INFO_OUT(newContext, nestedInfo.newContext);
		IR_INFO_OUT(touchedMainContext, nestedInfo.touchedMainContext || immediateInfo.touchedMainContext);
		return fullReference;
	}
	else {
		return this;
	}
}

void piranha::ParserStructure::resolveDefinitions() {
	if (m_definitionsResolved) return;

	// Resolve components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->resolveDefinitions();
	}

	_resolveDefinitions();

	m_definitionsResolved = true;
}

void piranha::ParserStructure::checkReferences(IrContextTree *inputContext) {
	// Check components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->checkReferences(inputContext);
	}

	if (m_checkReferences) {
		IrReferenceQuery query;
		query.inputContext = inputContext;
		query.recordErrors = true;
		IrReferenceInfo info;

		ParserStructure *reference = getReference(query, &info);

		if (info.err != nullptr) {
			getParentUnit()->addCompilationError(info.err);
		}
	}
}

void piranha::ParserStructure::checkInstantiation() {
	// Check components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->checkInstantiation();
	}

	_checkInstantiation();
}

void piranha::ParserStructure::validate() {
	if (m_validated) return;

	// Validate components
	int componentCount = getComponentCount();
	for (int i = 0; i < componentCount; i++) {
		m_components[i]->validate();
	}

	_validate();

	m_validated = true;
}

void piranha::ParserStructure::_validate() {
	/* void */
}

void piranha::ParserStructure::_checkInstantiation() {
	/* void */
}

void piranha::ParserStructure::writeReferencesToFile(std::ofstream &file, IrContextTree *context, int tabLevel) {
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

	IrReferenceInfo info;
	IrReferenceQuery query;
	query.inputContext = context;
	query.recordErrors = false;

	IrNode *asNode = getAsNode();
	ParserStructure *immediateReference = getImmediateReference(query, &info);

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

void piranha::ParserStructure::_resolveDefinitions() {
	/* void */
}

piranha::ParserStructure *piranha::ParserStructure::resolveLocalName(const std::string &name) const {
	return nullptr;
}

bool piranha::ParserStructure::allowsExternalAccess() const {
	IrVisibility visibility = (m_visibility == IrVisibility::DEFAULT) ? m_defaultVisibility : m_visibility;
	return visibility == IrVisibility::PUBLIC;
}

piranha::IrCompilationUnit *piranha::ParserStructure::getParentUnit() const {
	if (m_parentUnit == nullptr) return m_logicalParent->getParentUnit();
	else return m_parentUnit;
}
