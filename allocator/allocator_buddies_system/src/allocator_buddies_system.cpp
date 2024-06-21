#include "../include/allocator_buddies_system.h"
#include <cmath>


void* allocator_buddies_system::allocate(size_t value_size, size_t values_count) {
    char* base_ptr = reinterpret_cast<char*>(this->_memory);
    logger** logger_ptr = reinterpret_cast<logger**>(base_ptr - 2 * sizeof(size_t) - sizeof(logger*));

    (*logger_ptr)->debug("starting allocation process");

    size_t size = value_size * values_count;

    if (size == 0) {
        return nullptr;
    }

    size_t actual_size_with_align = 16; // default 16 bytes alignment
    size += sizeof(size_t) + sizeof(bool); // Adjust for block size and state
    size = (size + actual_size_with_align - 1) & ~(actual_size_with_align - 1);
    while (size > actual_size_with_align) {
        actual_size_with_align <<= 1;
    }

    size_t* block = find_free_block(size);

    if (block == nullptr) {
        merge_block();
        block = find_free_block(size);
    }

    if (block != nullptr) {
        split_block(block, size);
        *reinterpret_cast<bool*>(block + 1) = false;
        return reinterpret_cast<void*>(reinterpret_cast<char*>(block) + sizeof(size_t) + sizeof(bool));
    }

    return nullptr;
}

size_t* allocator_buddies_system::find_free_block(size_t size_block) {
    size_t* current = reinterpret_cast<size_t*>(this->_memory);
    size_t* end = reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) + *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - sizeof(size_t) * 2 - sizeof(logger*)));

    while (current < end) {
        size_t size = *current;
        if (size >= size_block && *reinterpret_cast<bool*>(current + 1)) {
            return current;
        }
        current = reinterpret_cast<size_t*>(reinterpret_cast<char*>(current) + size);
    }

    return nullptr;
}

void allocator_buddies_system::split_block(size_t* block, size_t required_size) {
    size_t size = *block;
    while (size > required_size) {
        size_t half_size = size >> 1;
        *block = half_size;
        *reinterpret_cast<bool*>(block + 1) = true;
        block = reinterpret_cast<size_t*>(reinterpret_cast<char*>(block) + half_size);
        *block = half_size;
        *reinterpret_cast<bool*>(block + 1) = true;
        size = half_size;
    }
}

void allocator_buddies_system::deallocate(void* ptr) {
    if (ptr != nullptr) {
        size_t* block = reinterpret_cast<size_t*>(reinterpret_cast<char*>(ptr) - sizeof(size_t) - sizeof(bool));
        *reinterpret_cast<bool*>(block + 1) = true;
    }
}

void allocator_buddies_system::merge_block() {
    size_t* current = reinterpret_cast<size_t*>(this->_memory);
    size_t* end = reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) + *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - sizeof(size_t) * 2 - sizeof(logger*)));

    while (current < end) {
        size_t size = *current;
        size_t* next = reinterpret_cast<size_t*>(reinterpret_cast<char*>(current) + size);

        if (next < end && *reinterpret_cast<bool*>(current + 1) && *reinterpret_cast<bool*>(next + 1) && *current == *next) {
            *current = (size << 1);
            *reinterpret_cast<bool*>(current + 1) = true;
        } else {
            current = next;
        }
    }
}
void allocator_buddies_system::set_fit_mode(fit_mode mode){

}


