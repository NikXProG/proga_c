#include "../include/allocator_sorted_list.h"


void* allocator_sorted_list::allocate(size_t value_size, size_t values_count){

    block_pointer_t block;

    logger** logger_ptr = reinterpret_cast<logger**>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) -  sizeof(logger*) - sizeof(fit_mode));
    
    (*logger_ptr)->debug("starting allocation process");

    //fit_mode_key
    switch(*reinterpret_cast<fit_mode*>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) - sizeof(fit_mode))){
        case fit_mode::first_fit:
            block = allocator_sorted_list::allocate_first_fit(value_size, values_count);
            break;
        case fit_mode::the_best_fit:
            block = allocator_sorted_list::allocate_best_fit(value_size, values_count);
            break;        
        case fit_mode::the_worst_fit:
            block = allocator_sorted_list::allocate_best_fit(value_size, values_count);
            break;   
    }

    (*logger_ptr)->debug("Allocation process completed");

    return block;
}

void allocator_sorted_list::set_fit_mode(fit_mode mode) {
    *reinterpret_cast<fit_mode*>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) - sizeof(fit_mode)) = mode;
}

void allocator_sorted_list::deallocate(void* ptr) {

    logger** logger_ptr = reinterpret_cast<logger**>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) -  sizeof(logger*) - sizeof(fit_mode) );

    if (ptr == nullptr) {
        (*logger_ptr)->error("Attempted to deallocate nullptr");
        throw std::invalid_argument("Attempted to deallocate nullptr");
    }

    if ( *reinterpret_cast<size_t*>(reinterpret_cast<bool*>(this->_memory) - sizeof(size_t)) != std::hash<std::string>()(get_typename()) ){
        (*logger_ptr)->error("Attempting to free a pointer that is not owned by an allocator");        
        throw std::logic_error("Attempting to free a pointer that is not owned by an allocator");
    }
    

    (*logger_ptr)->debug("Starting deallocation process");

    // Найти указатель на статусный байт
    bool* current_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<size_t*>(ptr) - 1) - 1;

    // Печать информации о деаллоцированном блоке
    size_t* current_size_ptr = reinterpret_cast<size_t*>(current_status_ptr + 1);


    if (*current_status_ptr != false){
            
        // Получение данных блока в виде массива байт
        const unsigned char* block = static_cast<const unsigned char*>(ptr);
        size_t size = *current_size_ptr;

        std::ostringstream oss;
        oss << "Memory block content (byte array representation):" << std::endl;
        oss << std::hex << std::setfill('0');

        for (size_t i = 0; i < size; ++i) {
            if (i % 16 == 0) {
                if (i != 0)
                    oss << std::endl;
                oss << std::setw(8) << i << ": ";
            }
            oss << std::setw(2) << static_cast<int>(block[i]) << " ";
        }

        (*logger_ptr)->debug(oss.str());
     
        *current_status_ptr = false;  // Пометить блок как свободный

        (*logger_ptr)->debug( "awail|" + std::to_string(*current_size_ptr) + "b block status");

        // Слияние с предыдущими и следующими свободными блоками
        // Найти предыдущий блок
        bool* prev_status_ptr = nullptr;
        bool* temp_status_ptr = reinterpret_cast<bool*>(this->_memory);

        while (temp_status_ptr < current_status_ptr) {
            prev_status_ptr = temp_status_ptr;
            size_t* temp_size_ptr = reinterpret_cast<size_t*>(temp_status_ptr + 1);
            temp_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(temp_status_ptr) + sizeof(bool) + sizeof(size_t) + *temp_size_ptr);
        }

        // Если предыдущий блок свободен, объединяем с ним
        if (prev_status_ptr && *prev_status_ptr == false) {
            size_t* prev_size_ptr = reinterpret_cast<size_t*>(prev_status_ptr + 1);
            *prev_size_ptr += sizeof(bool) + sizeof(size_t) + *current_size_ptr;
            current_status_ptr = prev_status_ptr;
            current_size_ptr = prev_size_ptr;
            (*logger_ptr)->debug("Merged with previous block");
        }

        // Проверить следующий блок и объединить, если он свободен
        bool* next_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(current_status_ptr) + sizeof(bool) + sizeof(size_t) + *current_size_ptr);
        size_t total_memory_size = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - 2 * sizeof(size_t));
        if (next_status_ptr < reinterpret_cast<bool*>(this->_memory) + total_memory_size && *next_status_ptr == false) {
            size_t* next_size_ptr = reinterpret_cast<size_t*>(next_status_ptr + 1);
            *current_size_ptr += sizeof(bool) + sizeof(size_t) + *next_size_ptr;
            (*logger_ptr)->debug("Merged with previous block");
        }
        
        (*logger_ptr)->debug("Deallocation process completed");
    }




}

void* allocator_sorted_list::allocate_worst_fit(size_t v_size, size_t v_count) {

    logger** logger_ptr = reinterpret_cast<logger**>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) - sizeof(fit_mode) - sizeof(logger*));

    (*logger_ptr)->trace("starting allocation process");
    
    size_t required_size = v_size * v_count;
    bool* worst_fit_status_ptr = nullptr;
    size_t* worst_fit_size_ptr = nullptr;
    size_t worst_fit_size = 0;

    bool* current_status_ptr = reinterpret_cast<bool*>(this->_memory);
    size_t total_memory_size = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - 2 * sizeof(size_t));

   
    while (reinterpret_cast<char*>(current_status_ptr) < reinterpret_cast<char*>(this->_memory) + total_memory_size) {
        bool current_status = *current_status_ptr;
        size_t* current_size_ptr = reinterpret_cast<size_t*>(current_status_ptr + 1);
        size_t current_size = *current_size_ptr;


        if (!current_status && current_size >= required_size && current_size > worst_fit_size) {
            // Найден худший подходящий блок
            worst_fit_status_ptr = current_status_ptr;
            worst_fit_size_ptr = current_size_ptr;
            worst_fit_size = current_size;
        }

        // Переход к следующему блоку
        current_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(current_status_ptr) + sizeof(bool) + sizeof(size_t) + current_size);
    }

    if (worst_fit_status_ptr) {
        // Если найден подходящий блок
        if (worst_fit_size > required_size) {
            // Разделение блока
            bool* new_block_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(worst_fit_status_ptr) + sizeof(bool) + sizeof(size_t) + required_size);
            *new_block_status_ptr = false;
            size_t* new_block_size_ptr = reinterpret_cast<size_t*>(new_block_status_ptr + 1);
            *new_block_size_ptr = worst_fit_size - required_size - sizeof(bool) - sizeof(size_t);

            (*logger_ptr)->information("Volume available for memory allocation in bytes: " + std::to_string(*new_block_size_ptr) + 'b');        

            // Обновление текущего блока
            *worst_fit_status_ptr = true;
            *worst_fit_size_ptr = required_size;
        } else {
            // Точный размер, просто помечаем блок как занятый
            *worst_fit_status_ptr = true;
        }

    
        // Вывод текущего состояния
        (*logger_ptr)->debug("occup|" + std::to_string(*reinterpret_cast<size_t*>(worst_fit_size_ptr)) + "b block status");
            
        (*logger_ptr)->trace("Allocation process completed");
        return reinterpret_cast<void*>(worst_fit_size_ptr + 1);
    }

    // Если не найден подходящий блок
    (*logger_ptr)->error( std::string("No suitable block found: ") + std::bad_alloc().what() );
    throw std::bad_alloc();

}

void* allocator_sorted_list::allocate_first_fit(size_t v_size, size_t v_count) {

    logger** logger_ptr = reinterpret_cast<logger**>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) - sizeof(fit_mode) - sizeof(logger*));

    (*logger_ptr)->trace("starting allocation process");
    // Общий размер требуемой памяти
    size_t required_size = v_size * v_count;

    // Указатель на текущий блок
    bool* current_status_ptr = reinterpret_cast<bool*>(this->_memory);

    // Получение общего размера памяти
    size_t total_memory_size = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - 2 * sizeof(size_t));



    while (reinterpret_cast<char*>(current_status_ptr) < reinterpret_cast<char*>(this->_memory) + total_memory_size) {
        bool current_status = *current_status_ptr;
        size_t* current_size_ptr = reinterpret_cast<size_t*>(current_status_ptr + 1);
        size_t current_size = *current_size_ptr;

        if (!current_status && current_size >= required_size) {
            // Найден подходящий блок
            if (current_size > required_size) {
                // Разделение блока
                bool* new_block_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(current_status_ptr) + sizeof(bool) + sizeof(size_t) + required_size);
                *new_block_status_ptr = false;
                size_t* new_block_size_ptr = reinterpret_cast<size_t*>(new_block_status_ptr + 1);
                *new_block_size_ptr = current_size - required_size - sizeof(bool) - sizeof(size_t);


                (*logger_ptr)->information("Volume available for memory allocation in bytes: " + std::to_string(*new_block_size_ptr) + 'b');        

                // Обновление текущего блока
                *current_status_ptr = true;
                *current_size_ptr = required_size;
            } else {
                // Точный размер, просто помечаем блок как занятый
                *current_status_ptr = true;
            }
    
            // Вывод текущего состояния
            (*logger_ptr)->debug("occup|" + std::to_string(*reinterpret_cast<size_t*>(current_size_ptr)) + "b block status");

            (*logger_ptr)->trace("Allocation process completed");
            return reinterpret_cast<void*>(current_size_ptr + 1);
        }

        // Переход к следующему блоку
        current_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(current_status_ptr) + sizeof(bool) + sizeof(size_t) + current_size);
    }

    // Если не найден подходящий блок
    (*logger_ptr)->error( std::string("No suitable block found: ") + std::bad_alloc().what() );
    throw std::bad_alloc();
}

void* allocator_sorted_list::allocate_best_fit(size_t v_size, size_t v_count) {

    logger** logger_ptr = reinterpret_cast<logger**>(reinterpret_cast<bool*>(this->_memory) - 2 * sizeof(size_t) - sizeof(fit_mode) - sizeof(logger*));

    (*logger_ptr)->trace("starting allocation process");
    
    size_t required_size = v_size * v_count;
    bool* best_fit_status_ptr = nullptr;
    size_t* best_fit_size_ptr = nullptr;
    size_t best_fit_size = std::numeric_limits<size_t>::max();

    bool* current_status_ptr = reinterpret_cast<bool*>(this->_memory);
    size_t total_memory_size = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(this->_memory) - 2 * sizeof(size_t));


    while (reinterpret_cast<char*>(current_status_ptr) < reinterpret_cast<char*>(this->_memory) + total_memory_size) {
        bool current_status = *current_status_ptr;
        size_t* current_size_ptr = reinterpret_cast<size_t*>(current_status_ptr + 1);
        size_t current_size = *current_size_ptr;
        

        if (!current_status && current_size >= required_size && current_size < best_fit_size) {
            // Найден лучший блок
            best_fit_status_ptr = current_status_ptr;
            best_fit_size_ptr = current_size_ptr;
            best_fit_size = current_size;
        }

        // Переход к следующему блоку
        current_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(current_status_ptr) + sizeof(bool) + sizeof(size_t) + current_size);

    }

    if (best_fit_status_ptr) {
        // Если найден подходящий блок
        if (best_fit_size > required_size) {
            // Разделение блока
            bool* new_block_status_ptr = reinterpret_cast<bool*>(reinterpret_cast<char*>(best_fit_status_ptr) + sizeof(bool) + sizeof(size_t) + required_size);
            *new_block_status_ptr = false;
            size_t* new_block_size_ptr = reinterpret_cast<size_t*>(new_block_status_ptr + 1);
            *new_block_size_ptr = best_fit_size - required_size - sizeof(bool) - sizeof(size_t);

            (*logger_ptr)->information("Volume available for memory allocation in bytes: " + std::to_string(*new_block_size_ptr) + 'b');        

            // Обновление текущего блока
            *best_fit_status_ptr = true;
            *best_fit_size_ptr = required_size;
        } else {
            // Точный размер, просто помечаем блок как занятый
            *best_fit_status_ptr = true;
        }


        (*logger_ptr)->trace("Allocation process completed");

        // Вывод текущего состояния
        (*logger_ptr)->debug("occup|" + std::to_string(*reinterpret_cast<size_t*>(best_fit_size_ptr)) + "b block status");

        return reinterpret_cast<void*>(best_fit_size_ptr + 1);
    }

    // Если не найден подходящий блок
    (*logger_ptr)->error( std::string("No suitable block found: ") + std::bad_alloc().what() );
    throw std::bad_alloc();
}

/*allocator_sorted_list::~allocator_sorted_list() {
    std::free(memory_);
}

void* allocator_sorted_list::allocate(size_t value_size, size_t values_count) {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t total_size = value_size * values_count;
    auto it = find_fit(total_size);
    if (it == free_blocks_.end()) {
        throw std::bad_alloc();
    }

    void* allocated_memory = it->first;
    size_t remaining_size = it->second - total_size;

    allocated_blocks_.emplace_back(allocated_memory, total_size);
    if (remaining_size > 0) {
        free_blocks_.emplace_back(static_cast<char*>(allocated_memory) + total_size, remaining_size);
    }
    free_blocks_.erase(it);
    log_memory_state();
    return allocated_memory;
}

void allocator_sorted_list::deallocate(void* at) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find_if(allocated_blocks_.begin(), allocated_blocks_.end(),
                           [at](const auto& block) { return block.first == at; });
    if (it == allocated_blocks_.end()) {
        return;
    }

    size_t block_size = it->second;
    void* block_start = it->first;

    allocated_blocks_.erase(it);

    merge_free_blocks(block_start, block_size);
    log_memory_state();
}

void allocator_sorted_list::set_fit_mode(fit_mode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    fit_mode_ = mode;
}

std::vector<std::pair<void*, size_t>>::iterator allocator_sorted_list::find_fit(size_t size) {
    switch (fit_mode_) {
        case fit_mode::first_fit:
            return std::find_if(free_blocks_.begin(), free_blocks_.end(),
                                [size](const auto& block) { return block.second >= size; });
        case fit_mode::the_best_fit:
            return std::min_element(free_blocks_.begin(), free_blocks_.end(),
                                    [size](const auto& a, const auto& b) {
                                        return (a.second >= size && a.second < b.second) || b.second < size;
                                    });
        case fit_mode::the_worst_fit:
            return std::max_element(free_blocks_.begin(), free_blocks_.end(),
                                    [size](const auto& a, const auto& b) {
                                        return (a.second >= size && a.second > b.second) || b.second < size;
                                    });
        default:
            return free_blocks_.end();
    }
}

void allocator_sorted_list::merge_free_blocks(void* start, size_t size) {
    free_blocks_.emplace_back(start, size);
    std::sort(free_blocks_.begin(), free_blocks_.end());
    for (auto it = free_blocks_.begin(); it != free_blocks_.end();) {
        auto next = std::next(it);
        if (next != free_blocks_.end() && static_cast<char*>(it->first) + it->second == next->first) {
            it->second += next->second;
            free_blocks_.erase(next);
        } else {
            ++it;
        }
    }
}

void allocator_sorted_list::log_memory_state() {
    std::string state;
    for (const auto& block : free_blocks_) {
        state += "avail " + std::to_string(block.second) + "|";
    }
    for (const auto& block : allocated_blocks_) {
        state += "occup " + std::to_string(block.second) + "|";
    }
    // Logging state to the console or a file can be added here
}*/