enable_language(ASM_NASM)

set(assembly_file julia_set.s)
set(JuliaSetSources julia_set.c julia_set.s)
set_source_files_properties(julia_set.s PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(julia_set.s PROPERTIES COMPILE_FLAGS "-f elf64")
add_library(JuliaSetLib STATIC ${JuliaSetSources})