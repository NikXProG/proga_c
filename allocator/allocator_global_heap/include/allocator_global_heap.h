#include "../../../logger/include/client_logger_builder.h"
#include "../../allocator/include/allocator.h"
#include "../../allocator/include/allocator_guardant.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <functional>
//allocator_global_heap

class allocator_global_heap : public allocator  {

private:

    logger * _logger;

public:

    allocator_global_heap(logger* logger) : _logger(logger) {}

    ~allocator_global_heap() noexcept override = default;

    allocator_global_heap(allocator_global_heap const &other) = delete;

    allocator_global_heap &operator=(allocator_global_heap const &other) = delete;

    allocator_global_heap(allocator_global_heap &&other) noexcept {
        _logger = other._logger;
        other._logger = nullptr;
    }

    allocator_global_heap &operator=(allocator_global_heap &&other) noexcept {
        if (this != &other) {
            _logger = other._logger;
            other._logger = nullptr;
        }
        return *this;
    }


    void* allocate(size_t value_size, size_t values_count) override;

    void deallocate(void* ptr) override;

    inline std::string get_typename() const noexcept { return "allocator_global_heap"; }


};
