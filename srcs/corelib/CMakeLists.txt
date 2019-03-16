project(corelib C)

add_library(corelib SHARED platform.h lex.c token.h token.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
