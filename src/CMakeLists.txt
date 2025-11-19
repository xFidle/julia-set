include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -pedantic -Werror -g")

add_subdirectory(queue)
add_subdirectory(tpool)
add_subdirectory(julia_set)
add_subdirectory(app)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE AppLib) 
