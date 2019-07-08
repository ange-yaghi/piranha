#include "ir_attribute_list.h"

#include "ir_attribute.h"

piranha::IrAttributeList::IrAttributeList() {
	/* void */
}

piranha::IrAttributeList::~IrAttributeList() {
	/* void */
}

void piranha::IrAttributeList::addAttribute(IrAttribute *attribute) {
	if (attribute != nullptr) {
		int index = getAttributeCount();

		m_attributes.push_back(attribute);
		attribute->setPosition(index);

		registerComponent(attribute);
	}
}

piranha::IrAttribute *piranha::IrAttributeList::getAttribute(IrAttributeDefinition *definition) const {
	int attributeCount = getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		IrAttribute *attribute = getAttribute(i);
		if (attribute->getAttributeDefinition() == definition) {
			return attribute;
		}
	}

	return nullptr;
}