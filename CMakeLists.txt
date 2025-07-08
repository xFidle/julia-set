cmake_minimum_required(VERSION 3.25)
project(JuliaSet) 
set(p_name ${PROJECT_NAME})

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src)

set(SOURCES main.c)
add_executable(p_name ${SOURCES})

target_link_libraries(p_name PRIVATE SDL3::SDL3) 
target_link_libraries(p_name PRIVATE SrcLib)
target_link_options(p_name PRIVATE "-Wl,-z,noexecstack")