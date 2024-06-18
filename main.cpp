#include <client_logger.h>
#include <allocator.h>
#include <allocator_global_heap.h>
#include <allocator_sorted_list.h>
#include <allocator_with_fit_mode.h>
int main() {


    //loggers

    // Создание и конфигурация логгера через Builder
    client_logger_builder* builder = new client_logger_builder();

    logger* logger1 = builder
        ->add_file_stream("logfile1.txt", logger::severity::information)
        ->add_console_stream(logger::severity::error)
        ->build();

    logger* logger2 = builder
        ->clear()
        ->add_file_stream("logfile1.txt", logger::severity::error)
        ->add_file_stream("logfile2.txt", logger::severity::trace)
        ->add_console_stream(logger::severity::debug)
        ->build();

    // Использование логгеров
    /*logger1->information("This is an info message.");
    logger1->error("This is an error message.");

    logger2->trace("This is a trace message.");
    logger2->error("This is another error message.");
    logger2->debug("This is another error message.");*/

    delete logger1;
    delete logger2;

    logger* logger_allocator = builder
        ->clear()
        ->add_file_stream("log_warning.txt", logger::severity::warning)
        ->add_file_stream("log_debug.txt", logger::severity::debug)
        ->add_file_stream("log_trace.txt", logger::severity::trace)
        ->add_file_stream("log_err.txt", logger::severity::error)
        ->add_file_stream("log_info.txt", logger::severity::information)
        ->add_console_stream(logger::severity::warning)
        ->add_console_stream(logger::severity::error)
        ->build();

    delete builder;

    //allocators

    try{
        //other allocator

        allocator* allocator1 = new allocator_global_heap(logger_allocator);

        int* array1 = reinterpret_cast<int*>(allocator1->allocate(sizeof(int), 10));

        for (int i = 0; i < 10; ++i) {
            array1[i] = i;
        }

        for (int i = 0; i < 10; ++i) {
            std::cout << array1[i] << " ";
        }

        std::cout << std::endl;

        allocator1->deallocate(array1);

        //other allocator

        allocator_with_fit_mode* allocator2 = new allocator_sorted_list(logger_allocator, sizeof(int), 200, allocator1);

        //method first_fit
        
        allocator2->set_fit_mode(allocator_with_fit_mode::fit_mode::first_fit);

        int* array2 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 20));


        for (int i = 0; i <20; i++){
            array2[i] = i;
        }     

        for (int i = 0; i < 20; ++i) {
            std::cout << array2[i] << " ";
        }

        std::cout << std::endl;

        allocator2->deallocate(array2);
        


        int* array3 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 20));
        
        for (int i = 0; i <20; i++){
            array3[i] = i;
        }     
        
        allocator2->deallocate(array2);
        allocator2->deallocate(array3);



        //method best_fit

        allocator2->set_fit_mode(allocator_with_fit_mode::fit_mode::the_best_fit);

        int* array4 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 20));
        
        for (int i = 0; i <20; i++){
            array4[i] = i;
        }


        //method worst_fit

        allocator2->set_fit_mode(allocator_with_fit_mode::fit_mode::the_worst_fit);

        int* array5 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 21));

        for (int i = 0; i <20; i++){
            array5[i] = i;
        }
        int* array6 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 21));
        int* array7 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 21));
        int* array8 =  reinterpret_cast<int*>(allocator2->allocate(sizeof(int), 21));
        
        //other allocator

        delete allocator2;
        delete allocator1;


    }catch(std::logic_error& ex){
        std::cout << ex.what() << std::endl;
        return -1;
        
    }catch(std::bad_alloc& ex){
        std::cout << ex.what() << std::endl;
        return -2;
        
    }catch (std::exception& ex){
        std::cout << ex.what() << std::endl;
        return -2;        
    }

    delete logger_allocator;
    
    return 0;
}