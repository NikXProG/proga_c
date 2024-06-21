cmake_minimum_required(VERSION 3.10.0)
project(mpm_labs_allctr VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(allocator)
add_subdirectory(allocator_global_heap)
add_subdirectory(allocator_sorted_list)
add_subdirectory(allocator_buddies_system)

