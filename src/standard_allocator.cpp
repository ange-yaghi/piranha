#include "../include/standard_allocator.h"

#include <stdlib.h>

piranha::StandardAllocator *piranha::StandardAllocator::s_global = nullptr;

piranha::StandardAllocator *piranha::StandardAllocator::Global() {
    if (s_global == nullptr) return s_global = new StandardAllocator;
    else return s_global;
}

piranha::StandardAllocator::StandardAllocator() {
    m_allocationLedger = 0;

    m_maxUsage = 0;
    m_currentUsage = 0;
}

piranha::StandardAllocator::~StandardAllocator() {
    assert(m_allocationLedger == 0);
    assert(m_currentUsage == 0);
}

void piranha::StandardAllocator::initialize() {
    m_allocationLedger = 0;

    m_maxUsage = 0;
    m_currentUsage = 0;
}
