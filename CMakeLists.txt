cmake_minimum_required(VERSION 3.10.0)
project(mpm_labs VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(logger)
add_subdirectory(allocator)

add_executable(mpm_labs main.cpp)

target_link_libraries(mpm_labs PUBLIC mpm_labs_log)
target_link_libraries(mpm_labs PUBLIC mpm_labs_allctr_allctr_gbl_hp)
target_link_libraries(mpm_labs PUBLIC mpm_labs_allctr_allctr)
target_link_libraries(mpm_labs PUBLIC mpm_labs_allctr_allctr_srtd_lst)