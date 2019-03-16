project(corelib C)

add_library(corelib SHARED platform.h lex.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
