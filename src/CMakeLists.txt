include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -pedantic -Werror -g")
set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=bugprone-*,clang-analyzer-*,redability-*")

add_subdirectory(queue)
add_subdirectory(tpool)
add_subdirectory(julia_set)
add_subdirectory(app)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE AppLib) 