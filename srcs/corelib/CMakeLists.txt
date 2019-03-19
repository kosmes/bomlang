set(CORELIB_VERSION_MAJOR "0")
set(CORELIB_VERSION_MINOR "0")
set(CORELIB_VERSION_PATCH "2")
set(CORELIB_VERSION_POSTFIX "알파")

set(CORELIB_VERSION "${CORELIB_VERSION_MAJOR}.${CORELIB_VERSION_MINOR}.${CORELIB_VERSION_PATCH}")

project(corelib
        VERSION ${CORELIB_VERSION}
        DESCRIPTION "The core library of programing language bom"
        LANGUAGES C)

add_library(corelib SHARED platform.h token.h token.c syntax.h syntax.c error.h error.c buf.h buf.c node.h node.c
        parser.h parser.c script.h script.c compiler.h compiler.c runtime.h visitor.h vm.h vm.c unicode.h unicode.c)
target_compile_definitions(corelib PRIVATE -DEXPORTS)
target_include_directories(corelib PUBLIC .)