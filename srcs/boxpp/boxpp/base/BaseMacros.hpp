#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#ifdef PLATFORM_WINDOWS
#undef PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_LINUX
#undef PLATFORM_LINUX
#endif

#ifdef PLATFORM_UNIX
#undef PLATFORM_UNIX
#endif

#ifdef PLATFORM_APPLE
#undef PLATFORM_APPLE
#endif

#ifdef PLATFORM_POSIX
#undef PLATFORM_POSIX
#endif

#ifdef PLATFORM_ANDROID
#undef PLATFORM_ANDROID
#endif

#ifdef PLATFORM_BITNESS
#undef PLATFORM_BITNESS
#endif

// ---------------------- BOX_NOT_COMPILED
#if defined(__INTELLISENSE__)
#	define BOX_NOT_COMPILED 1
#else
#	define BOX_NOT_COMPILED 0
#endif

// ---------------------- BOX_DEBUG
#if defined(_DEBUG)
#	define BOX_DEBUG		1
#else
#	define BOX_DEBUG		0
#endif

// ---------------------- PLATFORM_*
#if defined(_WIN64) || defined(_WIN32) || defined(__MINGW32__) \
	|| defined(__MINGW64__) || defined(__CYGWIN__)
#	define PLATFORM_WINDOWS	1
#endif

#if defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
#	define PLATFORM_LINUX	1
#elif defined(__unix__)
#	define PLATFORM_UNIX	1
#endif

#ifdef __ANDROID__ 
#	define PLATFORM_ANDROID	1
#endif

#ifdef __APPLE__ 
#	define PLATFORM_APPLE	1
#endif

#ifdef _POSIX_VERSION
#	define PLATFORM_POSIX	1
#endif

#ifndef PLATFORM_WINDOWS
#	define PLATFORM_WINDOWS	0
#endif

#ifndef PLATFORM_APPLE
#	define PLATFORM_APPLE	0
#endif

#ifndef PLATFORM_LINUX
#	define PLATFORM_LINUX	0
#endif

#ifndef PLATFORM_UNIX
#	define PLATFORM_UNIX	0
#endif

#ifndef PLATFORM_ANDROID
#	define PLATFORM_ANDROID	0
#endif

#ifndef PLATFORM_POSIX
#	define PLATFORM_POSIX	0
#endif

#if PLATFORM_POSIX || PLATFORM_LINUX || PLATFORM_ANDROID
#	define PLATFORM_POSIX_KIND	1
#else
#	define PLATFORM_POSIX_KIND	0
#endif

#if PLATFORM_UNIX || PLATFORM_LINUX || \
	PLATFORM_ANDROID || PLATFORM_POSIX || PLATFORM_APPLE
#	define PLATFORM_UNIX_KIND	1
#else
#	define PLATFORM_UNIX_KIND	0
#endif

#ifndef PLATFORM_BITNESS
#	if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) \
	|| defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) \
	|| defined(_M_IA64) || defined(_M_AMD64) || defined(__itanium__) || defined(__aarch64__)
#		define PLATFORM_BITNESS	64
#		ifndef __VOS__ 
#			define PLATFORM_LITTLE_ENDIAN	1
#		endif
#	elif defined(i386) || defined (__i386) || defined(__i386__) || defined(__i486__) \
	|| defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(_M_I86) \
	|| defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) \
	|| defined(__I86__) || defined(__386) || defined(__aarch64__)
#		define PLATFORM_BITNESS	32
#		define PLATFORM_LITTLE_ENDIAN	1
#	endif
#endif

#ifndef PLATFORM_LITTLE_ENDIAN
#define PLATFORM_LITTLE_ENDIAN	0
#else
#define PLATFORM_BIG_ENDIAN		0
#endif

// ---------------------- PLATFORM_UNICODE
#ifdef _UNICODE
#	define PLATFORM_UNICODE		1

/*	non-Windows system, _UNICODE macro is not set.
	and if _UNICODE required, its are like optional flag. */
#elif defined(WANNA_UNICODE)
#	define PLATFORM_UNICODE		1
#else
#	define PLATFORM_UNICODE		0
#endif

// ---------------------- PLATFORM_NATIVE_WCHAR

#if defined(_NATIVE_WCHAR_T_DEFINED) || defined(_WCHAR_T_DEFINED) || \
	defined(__SIZEOF_WCHAR_T__) || defined(__WCHAR_TYPE__)
	/* native supported wchar type. */
#	define PLATFORM_NATIVE_WCHAR	1
#	if !defined(__SIZEOF_WCHAR_T__) && PLATFORM_WINDOWS
#		define __SIZEOF_WCHAR_T__	2
#	endif
#	if !defined(__WCHAR_TYPE__)
#		define __WCHAR_TYPE__		wchar_t
#	endif
#else /* if no native support is being. */
#	define PLATFORM_NATIVE_WCHAR	0
#	if PLATFORM_UNICODE
#		undef PLATFORM_UNICODE
#		define PLATFORM_UNICODE		0
#	endif
#endif

#if !defined(__WCHAR_TYPE__) && defined(__SIZEOF_WCHAR_T__)
#if __SIZEOF_WCHAR_T__ > 2
#		define __WCHAR_TYPE__		int
#elif __SIZEOF_WCHAR_T__ > 1
#		define __WCHAR_TYPE__		short
#else
#		define __WCHAR_TYPE__		char
#endif

typedef __WCHAR_TYPE__	wchar_t;
#endif

// ---------------------- size_t
#ifdef _SIZE_T_DEFINED
#	ifndef __SIZE_TYPE__
#		define __SIZE_TYPE__ ::size_t
#	endif
#endif

#if PLATFORM_WINDOWS
#	ifndef __SIZE_TYPE__
#		if PLATFORM_BITNESS > 32
#			define __SIZE_TYPE__		unsigned __int64
#		else
#			define __SIZE_TYPE__		unsigned int
#		endif
#	endif
#endif

#ifdef __SIZE_TYPE__
#	define PLATFORM_SIZE_TYPE		__SIZE_TYPE__
#else
#	ifdef _SIZE_T_DEFINED
#		define __SIZE_TYPE__		::size_t
#	endif
#endif

#ifndef _SIZE_T_DEFINED
#	define _SIZE_T_DEFINED 1
typedef PLATFORM_SIZE_TYPE size_t;
#endif

// ---------------------- fastinline
#ifndef FASTINLINE
#	ifdef _MSC_VER
#		define FASTINLINE	__forceinline
#	else
#		define FASTINLINE	inline
#	endif
#endif

// --------------------- boxextern, no_mangle
#define BOXEXTERN		extern
#define NO_MANGLED		extern "C"

// --------------------- boxexport, boximport
#if PLATFORM_WINDOWS
#	ifdef _MSC_VER
#		define	BOXEXPORT		__declspec(dllexport)
#		define	BOXIMPORT		__declspec(dllimport)
#	elif defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__)
#		define	BOXEXPORT		__attribute__ ((dllexport))
#		define	BOXIMPORT		__attribute__ ((dllimport))
#	else
#		define	BOXEXPORT
#		define	BOXIMPORT
#	endif
#else
#	define	BOXEXPORT
#	define	BOXIMPORT
#endif

// --------------------- BOX_COMPILE_RUNTIME, BOX_COMPILE_BODY, BOX_COMPILE_EXECUTABLE
#ifdef __BOX_BODY__ /* when compiling boxpp.dll (or .so) */
#	define BOX_COMPILE_BODY			1
#else
#	define BOX_COMPILE_BODY			0
#endif

#if !BOX_COMPILE_BODY /* when compiling box'ed app. */
#	define BOX_COMPILE_EXECUTABLE	1
#else
#	define BOX_COMPILE_EXECUTABLE	0
#endif

// --------------------- BOX_COMPILE_SHARED_LIB, BOX_COMPILE_STATIC_LIB
#if defined(_USRDLL) || defined(__BOX_SHARED__) /* when compiling shared library.*/
#	define BOX_COMPILE_SHARED_LIB	1
#else
#	define BOX_COMPILE_SHARED_LIB	0
#endif

#if defined(_LIB) || defined(__BOX_STATIC__) /* when compiling static library.*/
#	define BOX_COMPILE_STATIC_LIB	1
#else
#	define BOX_COMPILE_STATIC_LIB	0
#endif

#if BOX_COMPILE_BODY
#	define BOXPP			BOXEXPORT
#else
#	define BOXPP			BOXIMPORT
#endif

// --------------------- boxalign(x), NO_PADDING
#ifdef _MSC_VER
#	if _MSC_VER < 1900
#		define __boxpp_alignas(x) __declspec( align( x ) )
#	else
#		define __boxpp_alignas(x) alignas(x)
#	endif
#	define NO_PADDING(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#	define __boxpp_alignas(x)	__attribute__ ((aligned (x)))
#	define NO_PADDING(...)	__VA_ARGS__ __attribute__ ((aligned (x)))
#endif

#define BOXALIGN(x) __boxpp_alignas(x)

#if PLATFORM_WINDOWS && defined (_MSC_VER)
#define BOX_STDCALL  __stdcall
#else
#define BOX_STDCALL __attribute__ ((stdcall))
#endif

#if PLATFORM_UNICODE
#define BOXTEXT(x)	L##x
#else
#define BOXTEXT(x)	x
#endif

#define BOX_TOSTR_2ND(a)	#a
#define BOX_TOSTR(a)		BOX_TOSTR_2ND(a)

#if PLATFORM_WINDOWS
#	define PLATFORM_EXT_SHARED		"dll"
#	define PLATFORM_EXT_STATIC		"lib"
#elif PLATFORM_LINUX || PLATFORM_UNIX
#	define PLATFORM_EXT_SHARED		"so"
#	define PLATFORM_EXT_STATIC		"a"
#elif PLATFORM_APPLE
#	define PLATFORM_EXT_SHARED		"dylib"
#	define PLATFORM_EXT_STATIC		"a"
#endif

/*	Auto-completion block for Visual-studio.
	Auto-completion blocks will not be really compiled. */
#if BOX_NOT_COMPILED
#undef PLATFORM_WINDOWS
#undef PLATFORM_LINUX
#undef PLATFORM_UNIX
#undef PLATFORM_APPLE
#undef PLATFORM_POSIX
#undef PLATFORM_ANDROID

#define PLATFORM_WINDOWS	1
#define PLATFORM_LINUX		1
#define PLATFORM_UNIX		1
#define PLATFORM_APPLE		1
#define PLATFORM_POSIX		1
#define PLATFORM_ANDROID	1

	/* skip parsing __attribute__ specifiers for gcc on VisualStudio. */
#define __attribute__(...)
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

#define BOX_CONCAT_2ND(Front, Back)	Front ## Back
#define BOX_CONCAT(Front, Back)		BOX_CONCAT_2ND(Front, Back)
