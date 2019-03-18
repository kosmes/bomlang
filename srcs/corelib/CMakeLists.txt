project(corelib C)

add_library(corelib SHARED platform.h token.h token.c syntax.h syntax.c error.h error.c buf.h buf.c node.h node.c
        parser.h parser.c script.h script.c compiler.h compiler.c runtime.h visitor.h vm.h vm.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
target_include_directories(corelib PUBLIC .)