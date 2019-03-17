project(corelib C)

add_library(corelib SHARED platform.h token.h token.c syntax.h syntax.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
