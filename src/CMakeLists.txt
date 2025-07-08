enable_language(C ASM_NASM)

set(assembly_file julia_set/julia_set.s)
set(JuliaSetSources julia_set/julia_set.c ${assembly_file})
set_source_files_properties(${assembly_file} PROPERTIES LANGUAGE ASM_NASM)
set_source_files_properties(${assembly_file} PROPERTIES COMPILE_FLAGS "-f elf64")

set(SDLResourcesSources sdl_resources/sdl_resources.c)

add_library(SrcLib STATIC ${JuliaSetSources} ${SDLResourcesSources})
set(INCLUDE_DIRS . ./julia_set ./sdl_resources)
target_include_directories(SrcLib PUBLIC ${INCLUDE_DIRS})