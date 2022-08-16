#ifndef PIRANHA_PATH_H
#define PIRANHA_PATH_H

#include <string>

#if USE_CXX17_FILESYSTEM
#include <filesystem>
#else // USE_CXX17_FILESYSTEM
// Forward Boost declaration
namespace boost {
    namespace filesystem {
        class path;
    } /* namespace filesystem */
} /* namespace boost */
#endif // USE_CXX17_FILESYSTEM

namespace piranha {

#if USE_CXX17_FILESYSTEM
    namespace filesystem = std::filesystem;
#else // USE_CXX17_FILESYSTEM
    namespace filesystem = boost::filesystem;
#endif // USE_CXX17_FILESYSTEM

    class Path {
    protected: Path(const filesystem::path &path);
    public:
        Path(const std::string &path);
        Path(const char *path);
        Path(const Path &path);
        Path();
        ~Path();

        std::string toString() const;

        void setPath(const std::string &path);
        bool operator==(const Path &path) const;
        Path append(const Path &path) const;

        void getParentPath(Path *path) const;

        const Path &operator =(const Path &b);

        std::string getExtension() const;
        std::string getStem() const;

        bool isAbsolute() const;
        bool exists() const;

    protected:
        filesystem::path *m_path;

    protected:
        const filesystem::path &getBoostPath() const { return *m_path; }
    };

} /* namespace piranha */

#endif /* PIRANHA_PATH_H */
