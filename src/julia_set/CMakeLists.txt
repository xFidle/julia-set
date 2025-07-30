enable_language(ASM_NASM)

set(JuliaSetSources julia_set.c julia_set.s)
set_source_files_properties(julia_set.s PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(julia_set.s PROPERTIES COMPILE_FLAGS "-f elf64")
add_library(JuliaSetLib STATIC ${JuliaSetSources})
target_link_options(JuliaSetLib PUBLIC "-Wl,-z,noexecstack")
target_include_directories(JuliaSetLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})