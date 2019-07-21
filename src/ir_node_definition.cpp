#include "../include/ir_node_definition.h"

#include "../include/ir_attribute_definition_list.h"
#include "../include/ir_attribute_definition.h"
#include "../include/ir_compilation_unit.h"
#include "../include/ir_node.h"
#include "../include/compilation_error.h"
#include "../include/ir_value.h"
#include "../include/ir_context_tree.h"
#include "../include/language_rules.h"

piranha::IrNodeDefinition::IrNodeDefinition(const IrTokenInfo_string &name) {
	m_name = name;
	registerToken(&name);

	// Node definitions are public by default
	setDefaultVisibility(IrVisibility::PUBLIC);
}

piranha::IrNodeDefinition::~IrNodeDefinition() {
	/* void */
}

void piranha::IrNodeDefinition::setBuiltinName(const IrTokenInfo_string &builtinName) {
	m_builtinName = builtinName;
	registerToken(&m_builtinName);
}

void piranha::IrNodeDefinition::setAttributeDefinitionList(IrAttributeDefinitionList *definitions) {
	m_attributes = definitions;
	registerComponent(definitions);
}

void piranha::IrNodeDefinition::setScopeToken(const IrTokenInfo_string &scopeToken) {
	m_scopeToken = scopeToken;
	registerToken(&scopeToken);
}

piranha::IrAttributeDefinition *piranha::IrNodeDefinition::getAttributeDefinition(const std::string &attributeName) const {
	if (m_attributes == nullptr) return nullptr;

	int attributeCount = m_attributes->getDefinitionCount();

	for (int i = 0; i < attributeCount; i++) {
		IrAttributeDefinition *definition = m_attributes->getDefinition(i);

		if (definition->getName() == attributeName) {
			return definition;
		}
	}

	return nullptr;
}

piranha::IrParserStructure *piranha::IrNodeDefinition::resolveName(const std::string &name) const {
	// Node definitions are not able to see variables outside of themselves for now
	return resolveLocalName(name);
}

const piranha::ChannelType *piranha::IrNodeDefinition::getChannelType() {
	if (m_rules == nullptr) return nullptr;

	if (isBuiltin()) {
		const ChannelType *immediateChannelType = m_rules->resolveChannelType(getBuiltinName());
		if (immediateChannelType != nullptr) return immediateChannelType;
	}

	IrNodeDefinition *aliasType = getAliasType();
	
	return (aliasType != this)
		? aliasType->getChannelType()
		: nullptr;
}

piranha::IrNodeDefinition *piranha::IrNodeDefinition::getAliasType() {
	if (m_attributes == nullptr) return this;

	IrAttributeDefinition *alias = m_attributes->getAliasOutput();
	if (alias == nullptr) return this;

	IrNodeDefinition *typeDefinition = alias->getTypeDefinition();
	if (typeDefinition == nullptr) return this;

	return typeDefinition->getAliasType();
}

int piranha::IrNodeDefinition::countSymbolIncidence(const std::string &name) const {
	if (name.empty()) return 0;

	int count = 0;
	if (m_attributes != nullptr) {
		int attributeCount = m_attributes->getDefinitionCount();
		for (int i = 0; i < attributeCount; i++) {
			IrAttributeDefinition *definition = m_attributes->getDefinition(i);

			if (definition->getName() == name) {
				count++;
			}
		}
	}

	if (m_body != nullptr) {
		int nodeCount = m_body->getItemCount();
		for (int i = 0; i < nodeCount; i++) {
			IrNode *node = m_body->getItem(i);
			if (node->getName() == name) {
				count++;
			}
		}
	}

	return count;
}

piranha::IrParserStructure *piranha::IrNodeDefinition::resolveLocalName(const std::string &name) const {
	if (m_attributes != nullptr) {
		int attributeCount = m_attributes->getDefinitionCount();
		for (int i = 0; i < attributeCount; i++) {
			IrAttributeDefinition *definition = m_attributes->getDefinition(i);

			if (definition->getName() == name) {
				return definition;
			}
		}
	}

	if (m_body != nullptr) {
		int nodeCount = m_body->getItemCount();
		for (int i = 0; i < nodeCount; i++) {
			IrNode *node = m_body->getItem(i);
			if (node->getName() == name) {
				return node;
			}
		}
	}

	return nullptr;
}

void piranha::IrNodeDefinition::_validate() {
	IrCompilationUnit *unit = getParentUnit();

	// Check that no symbol is used more than once
	if (m_attributes != nullptr) {
		int attributeCount = m_attributes->getDefinitionCount();
		for (int i = 0; i < attributeCount; i++) {
			IrAttributeDefinition *definition = m_attributes->getDefinition(i);
			int incidence = countSymbolIncidence(definition->getName());
			if (incidence > 1) {
				unit->addCompilationError(new CompilationError(*definition->getNameToken(),
					ErrorCode::SymbolUsedMultipleTimes));
			}
		}
	}

	if (m_body != nullptr) {
		int nodeCount = m_body->getItemCount();
		for (int i = 0; i < nodeCount; i++) {
			IrNode *node = m_body->getItem(i);
			int incidence = countSymbolIncidence(node->getName());
			if (incidence > 1) {
				unit->addCompilationError(new CompilationError(node->getNameToken(),
					ErrorCode::SymbolUsedMultipleTimes));
			}
		}
	}

	// Check that every output has a definition of the right type
	if (m_attributes != nullptr) {
		int attributeCount = m_attributes->getDefinitionCount();
		for (int i = 0; i < attributeCount; i++) {
			IrAttributeDefinition *definition = m_attributes->getDefinition(i);
			if (definition->getDirection() == IrAttributeDefinition::OUTPUT) {
				IrValue *value = definition->getDefaultValue();
				if (value != nullptr) {
					if (isBuiltin()) {
						unit->addCompilationError(new CompilationError(*definition->getNameToken(),
							ErrorCode::BuiltinOutputWithDefinition));
					}
				}
				else {
					if (!isBuiltin()) {
						unit->addCompilationError(new CompilationError(*definition->getNameToken(),
							ErrorCode::OutputWithNoDefinition));
					}
				}

				if (definition->typeInfoSpecified() && !isBuiltin()) {
					unit->addCompilationError(new CompilationError(*definition->getNameToken(),
						ErrorCode::StandardOutputWithType));
				}
			}
		}
	}
}
