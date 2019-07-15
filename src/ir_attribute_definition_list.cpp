#include "../include/ir_attribute_definition_list.h"

#include "../include/ir_attribute_definition.h"

piranha::IrAttributeDefinitionList::IrAttributeDefinitionList() {
	/* void */
}

piranha::IrAttributeDefinitionList::~IrAttributeDefinitionList() {
	/* void */
}

void piranha::IrAttributeDefinitionList::addDefinition(IrAttributeDefinition *definition) {
	if (definition != nullptr) {
		m_definitions.push_back(definition);
		registerComponent(definition);
	}
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getDefinition(int index, IrAttributeDefinition::DIRECTION direction) const {
	int totalCount = getDefinitionCount();
	int inputIndex = 0;
	for (int i = 0; i < totalCount; i++) {
		IrAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == direction) {
			if (inputIndex == index) {
				return definition;
			}

			inputIndex++;
		}
	}

	return nullptr;
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getInputDefinition(int index) const {
	return getDefinition(index, IrAttributeDefinition::INPUT);
}

int piranha::IrAttributeDefinitionList::getCount(IrAttributeDefinition::DIRECTION direction) const {
	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		IrAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == direction) {
			inputs++;
		}
	}

	return inputs;
}

int piranha::IrAttributeDefinitionList::getInputCount() const {
	return getCount(IrAttributeDefinition::INPUT);
}

int piranha::IrAttributeDefinitionList::getOutputCount() const {
	return getCount(IrAttributeDefinition::OUTPUT);
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getOutputDefinition(int index) const {
	return getDefinition(index, IrAttributeDefinition::OUTPUT);
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getOutputDefinition(const std::string &name) const {
	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		IrAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == IrAttributeDefinition::OUTPUT && name == definition->getName()) {
			return definition;
		}
	}

	return nullptr;
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getDefaultOutput() const {
	IrAttributeDefinition *firstOutput = nullptr;

	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		IrAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == IrAttributeDefinition::OUTPUT) {
			firstOutput = definition;

			if (definition->isDefault()) {
				return definition;
			}
		}
	}

	// If not explicit default is found, return the first output
	return firstOutput;
}
