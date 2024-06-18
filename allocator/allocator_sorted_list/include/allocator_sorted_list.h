#ifndef ALLOCATOR_SORTED_LIST_H
#define ALLOCATOR_SORTED_LIST_H 

#include <vector>
#include <mutex>
#include "../../allocator/include/allocator_with_fit_mode.h"
#include "../../../logger/include/client_logger_builder.h"

class allocator_sorted_list : public allocator_with_fit_mode {

private:

    void* _memory = nullptr;

public:

    allocator_sorted_list(logger* logger_object, size_t value_size, size_t values_count, allocator* allocator = nullptr){

        this->_memory = ((allocator == nullptr) ? (::operator new(value_size * values_count)) : allocator->allocate(value_size, values_count));
        
        //  allocator_meta [logger*][fit_mode][total_size][hash_type]

        logger** logger_obj = reinterpret_cast<logger**>(this->_memory);

        *logger_obj = logger_object;   

        fit_mode* fit_mode_key = reinterpret_cast<fit_mode*>(logger_obj + 1);
        
        *fit_mode_key = fit_mode::first_fit;   //default first_fit

        size_t* total_size = reinterpret_cast<size_t*>(fit_mode_key + 1);
        
        *total_size = value_size * values_count;

        *(total_size + 1) = std::hash<std::string>()(get_typename());
        

        // first_block_meta [status_occup][size_block]         
        bool* first_unoccured_block = reinterpret_cast<bool*>(total_size + 2);
        *first_unoccured_block = false;

        *reinterpret_cast<size_t*>(first_unoccured_block + 1) = value_size * values_count - 2*sizeof(size_t) - sizeof(logger*) - sizeof(fit_mode);
        
        this->_memory = reinterpret_cast<void*>(total_size+2);

    }
    
    ~allocator_sorted_list() noexcept override = default;

public:

    void set_fit_mode(fit_mode mode) override;

public:

    void* allocate(size_t value_size, size_t values_count) override;
    void deallocate(void* at) override;

public:

    allocator_sorted_list(const allocator_sorted_list& other) = delete;
    allocator_sorted_list& operator=(const allocator_sorted_list& other) = delete;

public:

    void * allocate_first_fit(size_t value_size, size_t values_count);
    void * allocate_best_fit(size_t value_size, size_t values_count);
    void * allocate_worst_fit(size_t value_size, size_t values_count);

public:

    inline std::string get_typename() const noexcept { return "allocator_sorted_list"; }

};

#endif // ALLOCATOR_SORTED_LIST_H