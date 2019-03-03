
add_library(corelib SHARED lex.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
