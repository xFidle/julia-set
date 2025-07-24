find_package(Threads REQUIRED)
set(QueueSources queue.c)
add_library(QueueLib STATIC ${QueueSources})
target_link_libraries(QueueLib PUBLIC Threads::Threads)