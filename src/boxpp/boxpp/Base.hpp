#ifndef __BOXPP_BASE_HPP__
#define __BOXPP_BASE_HPP__

#include <boxpp/Base.Normalize.hpp>

#define BOX_CONCAT_2ND(Front, Back)	Front ## Back
#define BOX_CONCAT(Front, Back)		BOX_CONCAT_2ND(Front, Back)

// Version header. (pre-include for getting version information only)
#define __BOXPP_VERSION_MACRO_ONLY
#include <boxpp/Version.hpp>

namespace boxpp
{
	typedef decltype(nullptr)	nullptr_t;

	/* common integer types. */
	typedef char				s8;
	typedef short				s16;
	typedef long				s32;
	typedef long long			s64;

	typedef unsigned char		u8;
	typedef unsigned short		u16;
	typedef unsigned long		u32;
	typedef unsigned long long	u64;

	typedef float				f32;
	typedef double				f64;

	/* integer data-base for querying type using their size. */
	namespace type_db {
		template<s8 size> struct TSizedInt;
		template<> struct TSizedInt<1> { using Signed = s8; using Unsigned = u8; };
		template<> struct TSizedInt<2> { using Signed = s16; using Unsigned = u16; };
		template<> struct TSizedInt<4> { using Signed = s32; using Unsigned = u32; };
		template<> struct TSizedInt<8> { using Signed = s64; using Unsigned = u64; };

		template<s8 size> using SignedInt = typename TSizedInt<size>::Signed;
		template<s8 size> using UnignedInt = typename TSizedInt<size>::Unsigned;
	}

	typedef s8 c8;
#if PLATFORM_NATIVE_WCHAR
	/* internal implementation for... */
	namespace __unicode_supports__ {
		template<s8 size = sizeof(wchar_t)> struct TUtfType;
		template<> struct TUtfType<2> {
			using UTF32 = s32;
			using UTF16 = wchar_t;
		};
		template<> struct TUtfType<4> {
			using UTF32 = wchar_t;
			using UTF16 = s16;
		};
	}

	typedef __unicode_supports__::TUtfType<>::UTF16 c16;
	typedef __unicode_supports__::TUtfType<>::UTF32 c32;

	namespace __unicode_supports__ {
		typedef wchar_t wide_t;
	}
#else
	typedef s16 c16;
	typedef s32 c32;
#endif

#	if PLATFORM_UNICODE
	typedef __unicode_supports__::wide_t char_t;
#else
	typedef char	char_t;
#	endif

	/* size_t type. */
#ifdef PLATFORM_SIZE_TYPE
	typedef PLATFORM_SIZE_TYPE size_t;
	typedef type_db::SignedInt<sizeof(size_t)> ssize_t;
#else
	typedef type_db::UnignedInt<sizeof(void*)> size_t;
	typedef type_db::SignedInt<sizeof(size_t)> ssize_t;
#endif
	
	/*
		Below System-spec'ed types are 
		for template specialization if required.
	*/

	enum class EEndianType {
		Unknown = 0,/* Unknown, undefined. */
		Little,		/* Little endian. */
		Big,		/* Big endian. */

		/* Compile-time system endian. */
		System =(PLATFORM_BIG_ENDIAN ? Big : 
				(PLATFORM_LITTLE_ENDIAN ? Little : Unknown))
	};

	enum class EPlatformType {
		Undefined = 0,/* Undefined system. */
		Windows,	/* Windows. */
		Linux,		/* Linux. (android also linux but, this is not for android) */
		Android,	/* Android. */
		Apple,		/* Apple kind. (iPhone, iMac, ...) */
		Unix,		/* Unix kind. (commonly known unix like FreeBSD, ...) */

		/* Compile-time system platform. */
		System =(PLATFORM_WINDOWS ? Windows : (PLATFORM_ANDROID ? Android :
				(PLATFORM_LINUX ? Linux : (PLATFORM_APPLE ? Apple :
				(PLATFORM_UNIX ? Unix : Undefined)))))
	};

	enum class EPlatformBitness {
		Byte = 0,	/* 8 bit. --> impossible, but meta purpose. */
		Word,		/* 16 bit. --> all most not. meta purpose. */
		Dword,		/* 32 bit. --> really used. */
		Qword,		/* 64 bit. --> really used. */

		/* Compile-time system bitness. */
		System =(PLATFORM_BITNESS >= 64 ? Qword : (PLATFORM_BITNESS >= 32 ? Dword : 
				(PLATFORM_BITNESS >= 16 ? Word : Byte)))
	};

	/* Determine target-system has posix api or not. */
	static constexpr bool HasPosixAPI = PLATFORM_POSIX_KIND;
}


#endif // !__BOXPP_BASE_HPP__
