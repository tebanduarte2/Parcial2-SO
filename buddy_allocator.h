#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstddef>

class BuddyAllocator {
private:
    void* memoriaBase;
    char* nextFreePosition;
    char* endOfMemory;
    size_t size;

public:
    BuddyAllocator(size_t size);
    ~BuddyAllocator();

    void* alloc(size_t size);
    void free(void* ptr);
};

#endif // BUDDY_ALLOCATOR_H