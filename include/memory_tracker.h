#ifndef PIRANHA_MEMORY_TRACKER_H
#define PIRANHA_MEMORY_TRACKER_H

#include <string>
#include <vector>

namespace piranha {

    class MemoryTracker {
    public:
        struct Allocation {
            std::string Filename;
            int Line;
            int Index;
            void *Address;

            bool Freed;
        };

    protected:
        static MemoryTracker *s_instance;

    public:
        MemoryTracker();
        ~MemoryTracker();

        static MemoryTracker *Get();

        bool Find(void *address, Allocation *allocation);

        void RecordAllocation(void *address, const std::string &filename, int line);
        void RecordFree(void *address);

        int CountLeaks() const;

    private:
        std::vector<Allocation> m_allocations;
    };

#define TRACK(address) \
    (TrackedAllocation(address, __FILE__, __LINE__))

#define FTRACK(address) \
    (TrackedFree(address))

    template <typename T>
    T *TrackedAllocation(T *address, const char *_filename, int line) {
        const std::string filename = _filename;
        MemoryTracker::Get()->RecordAllocation(address, filename, line);

        return address;
    }

    template <typename T>
    T *TrackedFree(T *address) {
        MemoryTracker::Get()->RecordFree(address);

        return address;
    }

} /* namespace piranha */

#endif /* PIRANHA_MEMORY_TRACKER_H */
