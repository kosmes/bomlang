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

#if defined(__WIN32__) || defined(_WIN32)
#   define PLATFORM PLATFORM_WIN32
#else
#   define PLATFORM PLATFORM_LINUX
#endif

#if defined(_DEBUG) || defined(DEBUG)
#   define DEBUG_MODE 1
#else
#   define DEBUG_MODE 0
#endif

#if PLATFORM == PLATFORM_WIN32
#   if COMPILER == COMPILER_MSVC
#		if defined(STATIC_LIB)
#			define DLL_EXPORT
#		else
#			if defined(EXPORTS)
#				define DLL_EXPORT __declspec(dllexport)
#			else
#				define DLL_EXPORT __declspec(dllimport)
#			endif
#		endif
#   else
#       define DLL_EXPORT __attribute__ ((visibility("default")))
#   endif
#else
#   define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

#endif /* PLATFORM_H */
