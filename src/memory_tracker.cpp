#include "../include/memory_tracker.h"

#include <assert.h>

piranha::MemoryTracker *piranha::MemoryTracker::s_instance = nullptr;

piranha::MemoryTracker::MemoryTracker() {
    /* void */
}

piranha::MemoryTracker::~MemoryTracker() {
    /* void */
}

piranha::MemoryTracker *piranha::MemoryTracker::get() {
    if (s_instance == nullptr) s_instance = new MemoryTracker;
    return s_instance;
}

void piranha::MemoryTracker::reset() {
    m_allocations = std::vector<Allocation>();
}

bool piranha::MemoryTracker::find(void *address, Allocation *target) {
    for (Allocation &allocation : m_allocations) {
        if (allocation.address == address) {
            *target = allocation;
            return true;
        }
    }

    return false;
}

void piranha::MemoryTracker::recordAllocation(
    void *address, const std::string &filename, int line) 
{
    for (Allocation &allocation : m_allocations) {
        if (allocation.address == address) {
            allocation.freed = false;
            allocation.line = line;
            allocation.filename = filename;
            return;
        }
    }

    Allocation allocation;
    allocation.address = address;
    allocation.filename = filename;
    allocation.freed = false;
    allocation.line = line;
    allocation.index = (int)m_allocations.size();

    m_allocations.push_back(allocation);
}

void piranha::MemoryTracker::recordFree(void *address) {
    for (Allocation &allocation : m_allocations) {
        if (allocation.address == address) {
            assert(!allocation.freed);

            allocation.freed = true;
        }
    }
}

int piranha::MemoryTracker::countLeaks() const {
    std::vector<Allocation> unfreed;

    int count = 0;
    for (const Allocation &allocation : m_allocations) {
        if (!allocation.freed) {
            ++count;

            unfreed.push_back(allocation);
        }
    }

    return count;
}
