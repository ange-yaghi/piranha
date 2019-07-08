#include "path.h"

#include <boost/filesystem.hpp>

piranha::Path::Path(const std::string &path) {
	m_path = nullptr;
	setPath(path);
}

piranha::Path::Path(const char *path) {
	m_path = nullptr;
	setPath(std::string(path));
}

piranha::Path::Path(const Path &path) {
	m_path = new boost::filesystem::path;
	*m_path = path.getBoostPath();
}

piranha::Path::Path() {
	m_path = nullptr;
}

piranha::Path::Path(const boost::filesystem::path &path) {
	m_path = new boost::filesystem::path;
	*m_path = path;
}

piranha::Path::~Path() {
	delete m_path;
}

std::string piranha::Path::toString() const {
	return m_path->string();
}

void piranha::Path::setPath(const std::string &path) {
	if (m_path != nullptr) delete m_path;

	m_path = new boost::filesystem::path(path);
}

bool piranha::Path::operator==(const Path &path) const {
	return boost::filesystem::equivalent(this->getBoostPath(), path.getBoostPath());
}

piranha::Path piranha::Path::append(const Path &path) const {
	return Path(getBoostPath() / path.getBoostPath());
}

void piranha::Path::getParentPath(Path *path) const {
	path->m_path = new boost::filesystem::path;
	*path->m_path = m_path->parent_path();
}

const piranha::Path &piranha::Path::operator=(const Path &b) {
	if (m_path != nullptr) delete m_path;

	m_path = new boost::filesystem::path;
	*m_path = b.getBoostPath();

	return *this;
}

std::string piranha::Path::getExtension() const {
	return m_path->extension().string();
}

std::string piranha::Path::getStem() const {
	return m_path->stem().string();
}

bool piranha::Path::isAbsolute() const {
	return m_path->is_complete();
}

bool piranha::Path::exists() const {
	return boost::filesystem::exists(*m_path);
}
