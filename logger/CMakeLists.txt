cmake_minimum_required(VERSION 3.10.0)
project(mpm_labs_log VERSION 0.1.0 LANGUAGES C CXX)


add_library(
        mpm_labs_log
        src/logger.cpp
        src/logger_builder.cpp
        src/client_logger.cpp
        src/client_logger_builder.cpp)
target_include_directories(
        mpm_labs_log
        PUBLIC
        ./include)
