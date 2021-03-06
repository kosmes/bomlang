#ifndef PLATFORM_H
#define PLATFORM_H

#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2

#define COMPILER_MSVC 1
#define COMPILER_GNUC 2
#define COMPILER_CLANG 3

#if defined(__clang__)
#   define COMPILER COMPILER_CLANG
#elif defined(__GNUC__)
#   define COMPILER COMPILER_GNUC
#elif defined(_MSC_VER)
#   define COMPILER COMPILER_MSVC
#else
#   pragma error "No supported compiler"
#endif

#if defined(__WIN32__) || defined(_WIN32_)
#   define PLATFORM PLATFORM_WIN32
#else
#   define PLATFORM PLATFORM_LINUX
#endif

#if PLATFORM == PLATFORM_WIN32
#else
#   define EXPORT __attribute__ ((visibility("default")))
#endif

#endif /* PLATFORM_H */
