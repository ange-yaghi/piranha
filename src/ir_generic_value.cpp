#include "../include/ir_generic_value.h"

#include "../include/ir_compilation_unit.h"
#include "../include/compilation_error.h"
#include "../include/ir_node_definition.h"
#include "../include/language_rules.h"

piranha::IrGenericValue::IrGenericValue(const IrTokenInfoSet<std::string, 2> &type) 
														: IrValue(IrValue::GENERIC) {
	m_typeInfo = type;
	m_typeDefinition = nullptr;
}

piranha::IrGenericValue::~IrGenericValue() {
	/* void */
}

const piranha::ChannelType *piranha::IrGenericValue::getImmediateChannelType() {
	if (m_typeDefinition != nullptr) {
		if (!m_typeDefinition->isBuiltin()) return nullptr;
		std::string builtinType = m_typeDefinition->getBuiltinName();

		const ChannelType *wholeType = m_rules->resolveChannelType(builtinType);
		return wholeType;
	}
	else return nullptr;
}

piranha::IrParserStructure *piranha::IrGenericValue::resolveLocalName(const std::string &name) const {
	if (m_typeDefinition != nullptr) {
		return m_typeDefinition->resolveLocalName(name);
	}
	else return nullptr;
}

void piranha::IrGenericValue::_resolveDefinitions() {
	int definitionCount = 0;
	IrNodeDefinition *definition = nullptr;
	IrCompilationUnit *unit = getParentUnit();

	const IrTokenInfo_string &libraryToken = m_typeInfo.data[0];
	const IrTokenInfo_string &typeToken = m_typeInfo.data[1];

	const std::string &library = libraryToken.data;
	const std::string &type = typeToken.data;

	if (libraryToken.specified) {
		if (!library.empty()) {
			definition = unit->resolveNodeDefinition(type, &definitionCount, library);
		}
		else {
			// Adding an empty library name means that the local scope must strictly be used
			definition = unit->resolveLocalNodeDefinition(type, &definitionCount);
		}
	}
	else {
		if (typeToken.specified) {
			definition = unit->resolveNodeDefinition(type, &definitionCount, "");
		}
		else {
			// Type information was not given so skip the rest of the function
			m_typeDefinition = nullptr;
			return;
		}
	}

	if (definitionCount > 0) {
		// TODO: log a warning when a node type is ambiguous
	}

	if (definition == nullptr) {
		unit->addCompilationError(new CompilationError(typeToken,
			ErrorCode::UndefinedNodeType));
		m_typeDefinition = nullptr;
	}

	else {
		m_typeDefinition = definition;
	}
}
