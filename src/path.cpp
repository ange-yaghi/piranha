#include "../include/path.h"

#include "../include/memory_tracker.h"

#if !USE_CXX17_FILESYSTEM
#include <boost/filesystem.hpp>
#endif // USE_CXX17_FILESYSTEM

piranha::Path::Path(const std::string &path) {
    m_path = nullptr;
    setPath(path);
}

piranha::Path::Path(const char *path) {
    m_path = nullptr;
    setPath(std::string(path));
}

piranha::Path::Path(const Path &path) {
    m_path = TRACK(new filesystem::path);
    *m_path = path.getBoostPath();
}

piranha::Path::Path() {
    m_path = nullptr;
}

piranha::Path::Path(const filesystem::path &path) {
    m_path = TRACK(new filesystem::path);
    *m_path = path;
}

piranha::Path::~Path() {
    if (m_path != nullptr) delete FTRACK(m_path);
}

std::string piranha::Path::toString() const {
    return m_path->string();
}

void piranha::Path::setPath(const std::string &path) {
    if (m_path != nullptr) delete FTRACK(m_path);

    m_path = TRACK(new filesystem::path(path));
}

bool piranha::Path::operator==(const Path &path) const {
    return filesystem::equivalent(this->getBoostPath(), path.getBoostPath());
}

piranha::Path piranha::Path::append(const Path &path) const {
    return Path(getBoostPath() / path.getBoostPath());
}

void piranha::Path::getParentPath(Path *path) const {
    path->m_path = TRACK(new filesystem::path);
    *path->m_path = m_path->parent_path();
}

const piranha::Path &piranha::Path::operator=(const Path &b) {
    if (m_path != nullptr) delete FTRACK(m_path);

    m_path = TRACK(new filesystem::path);
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
#if USE_CXX17_FILESYSTEM
    return m_path->is_absolute();
#else // USE_CXX17_FILESYSTEM
    return m_path->is_complete();
#endif // USE_CXX17_FILESYSTEM
}

bool piranha::Path::exists() const {
    return filesystem::exists(*m_path);
}
