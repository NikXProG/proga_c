cmake_minimum_required(VERSION 3.10.0)
project(mpm_labs_allctr_allctr_gbl_hp VERSION 0.1.0 LANGUAGES C CXX)

add_library(
    mpm_labs_allctr_allctr_gbl_hp
    src/allocator_global_heap.cpp)

target_include_directories(
    mpm_labs_allctr_allctr_gbl_hp
    PUBLIC
    ./include)

set_target_properties(
    mpm_labs_allctr_allctr_gbl_hp PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION 1.0
    DESCRIPTION "allocator interface library")