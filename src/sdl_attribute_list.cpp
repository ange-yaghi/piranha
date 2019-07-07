#include <sdl_attribute_list.h>

#include <sdl_attribute.h>

piranha::SdlAttributeList::SdlAttributeList() {
	/* void */
}

piranha::SdlAttributeList::~SdlAttributeList() {
	/* void */
}

void piranha::SdlAttributeList::addAttribute(SdlAttribute *attribute) {
	if (attribute != nullptr) {
		int index = getAttributeCount();

		m_attributes.push_back(attribute);
		attribute->setPosition(index);

		registerComponent(attribute);
	}
}

piranha::SdlAttribute *piranha::SdlAttributeList::getAttribute(SdlAttributeDefinition *definition) const {
	int attributeCount = getAttributeCount();
	for (int i = 0; i < attributeCount; i++) {
		SdlAttribute *attribute = getAttribute(i);
		if (attribute->getAttributeDefinition() == definition) {
			return attribute;
		}
	}

	return nullptr;
}
