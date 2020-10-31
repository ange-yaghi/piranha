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

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::
    getDefinition(int index, bool isInput) const 
{
    const int totalCount = getDefinitionCount();
    int inputIndex = 0;
    for (int i = 0; i < totalCount; i++) {
        IrAttributeDefinition *definition = m_definitions[i];
        if (definition->isInput() == isInput) {
            if (inputIndex == index) {
                return definition;
            }

            inputIndex++;
        }
    }

    return nullptr;
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::
    getInputDefinition(int index) const 
{
    return getDefinition(index, true);
}

int piranha::IrAttributeDefinitionList::getCount(
    IrAttributeDefinition::Direction direction) const 
{
    const int totalCount = getDefinitionCount();
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
    return 
        getCount(IrAttributeDefinition::Direction::Input) + 
        getCount(IrAttributeDefinition::Direction::Modify) +
        getCount(IrAttributeDefinition::Direction::Toggle);
}

int piranha::IrAttributeDefinitionList::getOutputCount() const {
    return getCount(IrAttributeDefinition::Direction::Output);
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::
    getOutputDefinition(int index) const 
{
    return getDefinition(index, false);
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::
    getOutputDefinition(const std::string &name) const 
{
    const int totalCount = getDefinitionCount();
    for (int i = 0; i < totalCount; i++) {
        IrAttributeDefinition *definition = m_definitions[i];
        if (definition->getDirection() != IrAttributeDefinition::Direction::Output) continue;

        if (name == definition->getName()) return definition;
    }

    return nullptr;
}

piranha::IrAttributeDefinition *piranha::IrAttributeDefinitionList::getAliasOutput() const {
    const int totalCount = getDefinitionCount();
    for (int i = 0; i < totalCount; i++) {
        IrAttributeDefinition *definition = m_definitions[i];
        if (definition->getDirection() == IrAttributeDefinition::Direction::Output) {
            if (definition->isAlias()) return definition;
        }
    }

    return nullptr;
}
