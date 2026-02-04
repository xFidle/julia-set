cmake_minimum_required(VERSION 3.25)
project(JuliaSet) 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src)
