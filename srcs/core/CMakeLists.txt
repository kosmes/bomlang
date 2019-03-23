set(CORELIB_VERSION_MAJOR "0")
set(CORELIB_VERSION_MINOR "0")
set(CORELIB_VERSION_PATCH "2")
set(CORELIB_VERSION_POSTFIX "베타")

set(CORELIB_VERSION "${CORELIB_VERSION_MAJOR}.${CORELIB_VERSION_MINOR}.${CORELIB_VERSION_PATCH}")

project(core
        VERSION ${CORELIB_VERSION}
        DESCRIPTION "The core library of programing language bom"
        LANGUAGES C)

add_library(core SHARED platform.h compiler/token.h compiler/token.c compiler/syntax.h compiler/syntax.c error.h error.c buf.h buf.c compiler/node.h compiler/node.c
        compiler/parser.h compiler/parser.c compiler/script.h compiler/script.c compiler/compiler.h compiler/compiler.c runtime.h visitor.h vm.h vm.c unicode.h unicode.c
        container/table.h container/table.c smem.h smem.c)
target_compile_definitions(core PRIVATE -DEXPORTS)
target_include_directories(core PUBLIC .)