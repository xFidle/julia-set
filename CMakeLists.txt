cmake_minimum_required(VERSION 3.25)
project(JuliaSet) 

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src)
