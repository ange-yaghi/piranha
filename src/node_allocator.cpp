#include "../include/node_allocator.h"

piranha::NodeAllocator::NodeAllocator() {
    /* void */
}

piranha::NodeAllocator::~NodeAllocator() {
    /* void */
}

void *piranha::NodeAllocator::allocate(int size) {
    return malloc(size);
}

void piranha::NodeAllocator::free(void *memory) {
    ::free(memory);
}
