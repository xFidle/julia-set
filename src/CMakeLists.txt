include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=bugprone-*,clang-analyzer-*,redability-*")

add_subdirectory(julia_set)
add_subdirectory(app)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE AppLib) 
target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-z,noexecstack")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)