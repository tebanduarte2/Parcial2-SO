#include "buddy_allocator.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// Constructor: asigna un bloque de memoria de tamaño especificado usando malloc.
BuddyAllocator::BuddyAllocator(size_t size) {
    this->size = size;
    memoriaBase = std::malloc(size);
    if (!memoriaBase) {
        cerr << "Error: No se pudo asignar memoria base con Buddy System.\n";
        exit(1);
    }
    nextFreePosition = static_cast<char*>(memoriaBase);
    endOfMemory = static_cast<char*>(memoriaBase) + size;
}

// Destructor: libera el bloque de memoria.
BuddyAllocator::~BuddyAllocator() {
    std::free(memoriaBase);
}

// Asigna un bloque de memoria del tamaño especificado.
void* BuddyAllocator::alloc(size_t size) {
    if (nextFreePosition + size > endOfMemory) {
        cerr << "Error: No hay suficiente memoria disponible.\n";
        return nullptr;
    }
    
    void* result = nextFreePosition;
    nextFreePosition += size;
    return result;
}

