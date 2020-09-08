#include "../include/memory_tracker.h"

piranha::MemoryTracker *piranha::MemoryTracker::s_instance = nullptr;

piranha::MemoryTracker::MemoryTracker() {
    /* void */
}

piranha::MemoryTracker::~MemoryTracker() {
    /* void */
}

piranha::MemoryTracker *piranha::MemoryTracker::Get() {
    if (s_instance == nullptr) s_instance = new MemoryTracker;
    return s_instance;
}

bool piranha::MemoryTracker::Find(void *address, Allocation *target) {
    for (Allocation &allocation : m_allocations) {
        if (allocation.Address == address) {
            *target = allocation;
            return true;
        }
    }

    return false;
}

void piranha::MemoryTracker::RecordAllocation(
    void *address, const std::string &filename, int line) 
{
    for (Allocation &allocation : m_allocations) {
        if (allocation.Address == address) {
            allocation.Freed = false;
            allocation.Line = line;
            allocation.Filename = filename;
            return;
        }
    }

    Allocation allocation;
    allocation.Address = address;
    allocation.Filename = filename;
    allocation.Freed = false;
    allocation.Line = line;
    allocation.Index = (int)m_allocations.size();

    if (allocation.Index == 0) {
        int breakHere = 0;
    }

    m_allocations.push_back(allocation);
}

void piranha::MemoryTracker::RecordFree(void *address) {
    for (Allocation &allocation : m_allocations) {
        if (allocation.Address == address) {
            allocation.Freed = true;
        }
    }
}

int piranha::MemoryTracker::CountLeaks() const {
    std::vector<Allocation> unfreed;

    int count = 0;
    for (const Allocation &allocation : m_allocations) {
        if (!allocation.Freed) {
            ++count;

            unfreed.push_back(allocation);
        }
    }

    return count;
}
