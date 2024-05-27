// Copyright (c) 2024 Mooshua. All rights reserved.

#ifndef DUOSCRIPT_COMMON_H
#define DUOSCRIPT_COMMON_H

//	Export API
#ifdef KE_WINDOWS
#	ifdef BUILDING_KALUA
#		define KA_API __declspec(dllexport)
#	else
#		define KA_API __declspecdllimport
#	endif
#else
#	ifdef BUILDING_KALUA
#		define KA_API __attribute__((__visibility__("default")))
#	else
#		define KA_API
#	endif
#endif

//	Method attributes
#ifdef KE_CXX_MSVC
#	define KA_INLINE inline __forceinline
#	define KA_NOINLINE __declspec(noinline)
#	define KA_UNREACHABLE() __assume(false)
#else
#	define KA_INLINE inline __attribute__((always_inline))
#	define KA_NOINLINE __attribute__((noinline))
#	define KA_UNREACHABLE() __builtin_unreachable()
#endif

#endif //DUOSCRIPT_COMMON_H
