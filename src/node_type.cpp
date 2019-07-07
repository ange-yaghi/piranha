#include <node_type.h>

piranha::NodeType::NodeType() {
	initialize("", nullptr);
}

piranha::NodeType::NodeType(const char *type, const NodeType *parent) {
	initialize(type, parent);
}

piranha::NodeType::~NodeType() {
	/* void */
}

void piranha::NodeType::initialize(const char *type, const NodeType *parent) { 
	m_typeString = type; 
	m_hash = generateHash(type); 
	m_parent = parent;
}

bool piranha::NodeType::isCompatibleWith(const NodeType &t) const {
	if (operator==(t)) {
		return true;
	}
	else {
		if (m_parent == nullptr) return false;
		else return m_parent->isCompatibleWith(t);
	}
}

bool piranha::NodeType::operator==(const NodeType &t) const {
	bool hashCheck = (m_hash == t.m_hash);
	if (!hashCheck) return false;
	else return (strcmp(t.m_typeString, m_typeString) == 0);
}

int piranha::NodeType::generateHash(const char *string) {
	// Simple hash for now
	int sum = 0, i = 0;
	while (string[i] != '\0') {
		sum += (int)string[i++];
	}

	return sum;
}
