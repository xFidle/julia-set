set(p_nane ${PROJECT_NAME})
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(julia_set)
add_subdirectory(app)

add_executable(p_name main.c)

target_link_libraries(p_name PRIVATE AppLib) 
target_link_options(p_name PRIVATE "-Wl,-z,noexecstack")
target_compile_options(p_name PRIVATE -Wall -Wextra -pedantic -Werror)