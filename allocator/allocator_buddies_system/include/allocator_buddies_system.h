#ifndef ALLOCATOR_BUDDIES_SYSTEM_H
#define ALLOCATOR_BUDDIES_SYSTEM_H

#include <vector>
#include "../../allocator/include/allocator_with_fit_mode.h"
#include "../../../logger/include/client_logger_builder.h"

class allocator_buddies_system : public allocator_with_fit_mode {

private:

    void* _memory = nullptr;

public:

    allocator_buddies_system(logger* logger_object, size_t value_size, size_t values_count, allocator* allocator = nullptr) {
        size_t total_size = value_size * values_count;

        // Allocate memory
        this->_memory = (allocator == nullptr) ? ::operator new(total_size + 2 * sizeof(size_t) + sizeof(logger*)) : allocator->allocate(value_size, values_count);

        // Meta information
        char* base_ptr = reinterpret_cast<char*>(this->_memory);
        logger** logger_ptr = reinterpret_cast<logger**>(base_ptr);
        *logger_ptr = logger_object;

        size_t* total_size_ptr = reinterpret_cast<size_t*>(logger_ptr + 1);
        *total_size_ptr = total_size;

        size_t* hash_type_ptr = total_size_ptr + 1;
        *hash_type_ptr = std::hash<std::string>()(get_typename());

        // First block metadata
        size_t* first_block = reinterpret_cast<size_t*>(hash_type_ptr + 1);
        *first_block = total_size - 2 * sizeof(size_t) - sizeof(logger*);
        bool* first_block_status = reinterpret_cast<bool*>(first_block + 1);
        *first_block_status = true;

        // Adjust memory pointer
        this->_memory = reinterpret_cast<void*>(first_block);
    }
    
    ~allocator_buddies_system() noexcept override = default;

public:

    void set_fit_mode(fit_mode mode) override;

public:

    void* allocate(size_t value_size, size_t values_count) override;
    void deallocate(void* at) override;

public:

    allocator_buddies_system(const allocator_buddies_system& other) = delete;
    allocator_buddies_system& operator=(const allocator_buddies_system& other) = delete;

public:

    void * allocate_first_fit(size_t value_size, size_t values_count);
    void * allocate_best_fit(size_t value_size, size_t values_count);
    void * allocate_worst_fit(size_t value_size, size_t values_count);

public:

    size_t* find_free_block(size_t size);
    void split_block(size_t* block, size_t required_size);
    void merge_block();

public:

    inline std::string get_typename() const noexcept { return "allocator_buddies_system"; }

};

#endif // ALLOCATOR_BUDDIES_SYSTEM_H