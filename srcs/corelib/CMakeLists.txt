project(corelib C)

add_library(corelib SHARED platform.h token.h token.c syntax.h syntax.c errno.h errno.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
