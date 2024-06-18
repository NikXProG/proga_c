#include "../include/allocator_global_heap.h"

void* allocator_global_heap::allocate(size_t value_size, size_t values_count){

        _logger->debug("starting allocation process");

        block_pointer_t block_ptr = nullptr;

        try {
            size_t totalSize = value_size * values_count + sizeof(size_t);

            block_ptr = ::operator new(totalSize); // Сохраняем указатель на блок
            _logger->debug("Memory allocation successful");
            // Записываем размер блока перед возвращаемым указателем
            *reinterpret_cast<size_t*>(block_ptr) = std::hash<std::string>()(get_typename());

        } catch (const std::bad_alloc& ex) {
            _logger->error( std::string("Failed to allocate memory: ") + ex.what() );
            throw std::bad_alloc(); // Пробрасываем исключение дальше
        }

        _logger->debug("Allocation process completed");


        return reinterpret_cast<void*>(reinterpret_cast<size_t*>(block_ptr) + 1);
}

void allocator_global_heap::deallocate(void *ptr) {
    
    _logger->debug("Starting deallocation process");

    if (ptr == nullptr) {
        _logger->error("Attempted to deallocate nullptr");
        throw std::invalid_argument("Attempted to deallocate nullptr");
    }

    size_t* metaTypeAllocator = reinterpret_cast<size_t*>(ptr) - 1;

    if (*metaTypeAllocator != std::hash<std::string>()(get_typename())){
        _logger->error("Attempting to free a pointer that is not owned by an allocator");        
        throw std::logic_error("Attempting to free a pointer that is not owned by an allocator");
    }

    _logger->trace("Deallocating memory block");
    
    void* block_ptr = reinterpret_cast<void*>(metaTypeAllocator);

    // Освобождение памяти
    ::operator delete(block_ptr);

    _logger->debug("Deallocation process completed");
}