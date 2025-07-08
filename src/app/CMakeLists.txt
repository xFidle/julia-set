set(AppSources app.c)

add_library(AppLib STATIC ${AppSources})
target_link_libraries(AppLib PRIVATE JuliaSetLib PUBLIC SDL3::SDL3)