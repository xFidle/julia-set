cmake_minimum_required(VERSION 3.25)
project(JuliaSet) 

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src)
