#include <sdl_attribute_definition_list.h>

#include <sdl_attribute_definition.h>

piranha::SdlAttributeDefinitionList::SdlAttributeDefinitionList() {
	/* void */
}

piranha::SdlAttributeDefinitionList::~SdlAttributeDefinitionList() {
	/* void */
}

void piranha::SdlAttributeDefinitionList::addDefinition(SdlAttributeDefinition *definition) {
	if (definition != nullptr) {
		m_definitions.push_back(definition);
		registerComponent(definition);
	}
}

piranha::SdlAttributeDefinition *piranha::SdlAttributeDefinitionList::getDefinition(int index, SdlAttributeDefinition::DIRECTION direction) const {
	int totalCount = getDefinitionCount();
	int inputIndex = 0;
	for (int i = 0; i < totalCount; i++) {
		SdlAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == direction) {
			if (inputIndex == index) {
				return definition;
			}

			inputIndex++;
		}
	}

	return nullptr;
}

piranha::SdlAttributeDefinition *piranha::SdlAttributeDefinitionList::getInputDefinition(int index) const {
	return getDefinition(index, SdlAttributeDefinition::INPUT);
}

int piranha::SdlAttributeDefinitionList::getCount(SdlAttributeDefinition::DIRECTION direction) const {
	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		SdlAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == direction) {
			inputs++;
		}
	}

	return inputs;
}

int piranha::SdlAttributeDefinitionList::getInputCount() const {
	return getCount(SdlAttributeDefinition::INPUT);
}

int piranha::SdlAttributeDefinitionList::getOutputCount() const {
	return getCount(SdlAttributeDefinition::OUTPUT);
}

piranha::SdlAttributeDefinition *piranha::SdlAttributeDefinitionList::getOutputDefinition(int index) const {
	return getDefinition(index, SdlAttributeDefinition::OUTPUT);
}

piranha::SdlAttributeDefinition *piranha::SdlAttributeDefinitionList::getOutputDefinition(const std::string &name) const {
	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		SdlAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == SdlAttributeDefinition::OUTPUT && name == definition->getName()) {
			return definition;
		}
	}

	return nullptr;
}

piranha::SdlAttributeDefinition *piranha::SdlAttributeDefinitionList::getDefaultOutput() const {
	SdlAttributeDefinition *firstOutput = nullptr;

	int totalCount = getDefinitionCount();
	int inputs = 0;
	for (int i = 0; i < totalCount; i++) {
		SdlAttributeDefinition *definition = m_definitions[i];
		if (definition->getDirection() == SdlAttributeDefinition::OUTPUT) {
			firstOutput = definition;

			if (definition->isDefault()) {
				return definition;
			}
		}
	}

	// If not explicit default is found, return the first output
	return firstOutput;
}
