project(corelib C)

add_library(corelib SHARED platform.h token.h token.c syntax.h syntax.c errno.h errno.c buf.h buf.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
target_include_directories(corelib PUBLIC .)