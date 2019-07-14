#include "../include/channel_type.h"

piranha::ChannelType::ChannelType() {
	initialize("", nullptr);
}

piranha::ChannelType::ChannelType(const char *type, const ChannelType *parent) {
	initialize(type, parent);
}

piranha::ChannelType::~ChannelType() {
	/* void */
}

void piranha::ChannelType::initialize(const char *type, const ChannelType *parent) {
	m_typeString = type; 
	m_hash = generateHash(type); 
	m_parent = parent;
}

bool piranha::ChannelType::isCompatibleWith(const ChannelType &t) const {
	if (operator==(t)) {
		return true;
	}
	else {
		if (m_parent == nullptr) return false;
		else return m_parent->isCompatibleWith(t);
	}
}

bool piranha::ChannelType::operator==(const ChannelType &t) const {
	bool hashCheck = (m_hash == t.m_hash);
	if (!hashCheck) return false;
	else return (strcmp(t.m_typeString, m_typeString) == 0);
}

int piranha::ChannelType::generateHash(const char *string) {
	// Simple hash for now
	int sum = 0, i = 0;
	while (string[i] != '\0') {
		sum += (int)string[i++];
	}

	return sum;
}
