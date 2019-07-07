#include <sdl_attribute.h>

#include <sdl_value.h>

piranha::SdlAttribute::SdlAttribute() {
	/* void */
}

piranha::SdlAttribute::SdlAttribute(const SdlTokenInfo_string &name, SdlValue *value) {
	m_name = name;
	m_value = value;
	m_position = -1;

	registerToken(&name);
	registerComponent(value);

	m_definition = nullptr;
}

piranha::SdlAttribute::SdlAttribute(SdlValue *value) {
	m_value = value;
	m_position = -1;

	registerComponent(value);
}

piranha::SdlAttribute::~SdlAttribute() {
	/* void */
}

void piranha::SdlAttribute::setValue(SdlValue *value) {
	m_value = value;
	registerComponent(value);
}

piranha::SdlParserStructure *piranha::SdlAttribute::getImmediateReference(const SdlReferenceQuery &query, SdlReferenceInfo *output) {
	SDL_INFO_OUT(err, nullptr);
	SDL_INFO_OUT(newContext, query.inputContext);
	SDL_INFO_OUT(failed, false);

	return m_value;
}
