cmake_minimum_required(VERSION 3.25)
project(JuliaSet) 
set(p_name ${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src)

add_executable(p_name main.c)

target_link_libraries(p_name PRIVATE AppLib) 
target_link_options(p_name PRIVATE "-Wl,-z,noexecstack")
target_compile_options(p_name PRIVATE -Wall -Wextra -pedantic -Werror)