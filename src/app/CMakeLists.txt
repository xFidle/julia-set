find_package(Threads REQUIRED)
set(AppSources app.c)
add_library(AppLib STATIC ${AppSources})
target_link_libraries(AppLib PRIVATE Threads::Threads PUBLIC JuliaSetLib SDL3::SDL3)