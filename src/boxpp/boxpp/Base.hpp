#ifndef __BOXPP_BASE_HPP__
#define __BOXPP_BASE_HPP__

#include <boxpp/Base.Normalize.hpp>

#define BOX_CONCAT_2ND(Front, Back)	Front ## Back
#define BOX_CONCAT(Front, Back)		BOX_CONCAT_2ND(Front, Back)

// Version header. (pre-include for getting version information only)
#define __BOXPP_VERSION_MACRO_ONLY
#include <boxpp/Version.hpp>

#include <new>
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

	typedef int					s32i;
	typedef unsigned int		u32i;

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

		template<typename Type>
		struct TNumberLimits
		{
			static constexpr Type Min = 0;
			static constexpr Type Max = 0;
			static constexpr Type Tolerant = 0;
			static constexpr Type Epsilon = 0;
		};

#define BOXPP_TYPEDB_NUMBER_LIMIT(Type, MinVal, MaxVal, TolerantVal, EpsilonVal) \
		template<> struct TNumberLimits<Type> { \
			static constexpr Type Min = MinVal; \
			static constexpr Type Max = MaxVal; \
			static constexpr Type Tolerant = TolerantVal; \
			static constexpr Type Epsilon = EpsilonVal; \
		}

		BOXPP_TYPEDB_NUMBER_LIMIT(s8, -127, 127, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(s16, -32768, 32767, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(s32, -2147483647 - 1, 2147483647, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(s64, -9223372036854775807 - 1, 9223372036854775807, 0, 1);

		BOXPP_TYPEDB_NUMBER_LIMIT(u8, 0, 0xffu, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(u16, 0, 0xffffu, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(u32, 0, 0xffffffffu, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(u64, 0, 0xfffffffffffffffful, 0, 1);

		BOXPP_TYPEDB_NUMBER_LIMIT(s32i, -2147483647 - 1, 2147483647, 0, 1);
		BOXPP_TYPEDB_NUMBER_LIMIT(u32i, 0, 0xffffffffu, 0, 1);
		
		BOXPP_TYPEDB_NUMBER_LIMIT(f32, 1.175494351e-38F, 3.402823466e+38F, 0.001f, 1.192092896e-07F);
		BOXPP_TYPEDB_NUMBER_LIMIT(f64, 2.2250738585072014e-308, 1.7976931348623158e+308, 0.0000001, 2.2204460492503131e-016);

	}

	typedef s8 c8, ansi_t;

#if PLATFORM_NATIVE_WCHAR
	/* internal implementation for... */
	namespace __unicode_supports__ {
		typedef wchar_t wide_t;

		template<s8 size = sizeof(wchar_t)> struct TCharType;
		template<> struct TCharType<2> {
			using Char32 = s32;
			using Char16 = wide_t;
		};
		template<> struct TCharType<4> {
			using Char32 = wide_t;
			using Char16 = s16;
		};
	}

	using wide_t = __unicode_supports__::wide_t;
	typedef __unicode_supports__::TCharType<>::Char16 c16;
	typedef __unicode_supports__::TCharType<>::Char32 c32;

	namespace type_db {
		BOXPP_TYPEDB_NUMBER_LIMIT(wide_t,
			TNumberLimits<typename TSizedInt<sizeof(wide_t)>::Signed>::Min,
			TNumberLimits<typename TSizedInt<sizeof(wide_t)>::Signed>::Max,
			TNumberLimits<typename TSizedInt<sizeof(wide_t)>::Signed>::Tolerant,
			TNumberLimits<typename TSizedInt<sizeof(wide_t)>::Signed>::Epsilon
		);
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
