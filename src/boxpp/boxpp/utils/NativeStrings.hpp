#include <boxpp.hpp>

#ifndef __BOXPP_NATIVESTRINGS_HPP__
#define __BOXPP_NATIVESTRINGS_HPP__

namespace boxpp
{
	namespace _ {
		/* All C* Series have same code position for bellow characters. */
		template<typename CharType, bool IsUtfSeries = IsUtfType<CharType>>
		struct TNativeStringConstants {
			static constexpr const CharType Null = '\0';

			static constexpr const CharType Space = ' ';
			static constexpr const CharType Tab = '\t';
			static constexpr const CharType Backspace = '\b';
			static constexpr const CharType LineFeed = '\n';
			static constexpr const CharType CarrageReturn = '\r';
			static constexpr const CharType Asteroid = '*';
			static constexpr const CharType Slash = '/';
			static constexpr const CharType Backslash = '\\';

			static constexpr const CharType Hipen = '-';
			static constexpr const CharType Underbar = '_';
			static constexpr const CharType At = '@';
			static constexpr const CharType Sharp = '#';
			static constexpr const CharType Question = '?';
			static constexpr const CharType Less = '<';
			static constexpr const CharType Greater = '>';

			static constexpr const CharType Collon = ':';
			static constexpr const CharType Semicollon = ';';

			static constexpr const CharType Alphabet(size_t Offset) { return "abcdefghijklmnopqrstuvwxyz"[Offset]; }
			static constexpr const CharType AlphabetUpper(size_t Offset) { return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[Offset]; }
			static constexpr const CharType Number(size_t Offset) { return "0123456789"[Offset]; }
		};

		template<typename CharType>
		struct TNativeStringConstants<CharType, true> {
			static constexpr const CharType Null = { '\0' };

			static constexpr const CharType Space = { ' ' };
			static constexpr const CharType Tab = { '\t' };
			static constexpr const CharType Backspace = { '\b' };
			static constexpr const CharType LineFeed = { '\n' };
			static constexpr const CharType CarrageReturn = { '\r' };
			static constexpr const CharType Asteroid = { '*' };
			static constexpr const CharType Slash = { '/' };
			static constexpr const CharType Backslash = { '\\' };

			static constexpr const CharType Hipen = { '-' };
			static constexpr const CharType Underbar = { '_' };
			static constexpr const CharType At = { '@' };
			static constexpr const CharType Sharp = { '#' };
			static constexpr const CharType Question = { '?' };
			static constexpr const CharType Less = { '<' };
			static constexpr const CharType Greater = { '>' };

			static constexpr const CharType Collon = { ':' };
			static constexpr const CharType Semicollon = { ';' };

			static constexpr const CharType Alphabet(size_t Offset) { return { "abcdefghijklmnopqrstuvwxyz"[Offset] }; }
			static constexpr const CharType AlphabetUpper(size_t Offset) { return { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[Offset] }; }
			static constexpr const CharType Number(size_t Offset) { return { "0123456789"[Offset] }; }
		};

		/* Common implementation. */
		template<typename CharType, bool IsUtfSeries = IsUtfType<CharType>>
		struct TNativeStrings
		{
			using Consts = TNativeStringConstants<CharType>;

			FASTINLINE static size_t Strlen(const CharType* String) {
				size_t Length = 0; while (String[Length]) Length++;
				return Length;
			}

			FASTINLINE static s32 Strcmp(const CharType* Left, const CharType* Right) {
				if (Left && Right) {
					while (*Left && *Right &&
						*Left == *Right)
					{
						++Left; ++Right;
					}

					return *Left - *Right;
				}

				return Left && !Right ? -1 : 1;
			}

			FASTINLINE static s32 Strncmp(const CharType* Left, const CharType* Right, 
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				if (Left && Right) {
					while (*Left && *Right &&
						*Left == *Right && Max)
					{
						++Left; ++Right;
						--Max;
					}

					return Max ? *Left - *Right : 0;
				}

				return Left && !Right ? -1 : 1;
			}

			FASTINLINE static void Strcpy(CharType* Left, const CharType* Right)
			{
				if (Left && Right) {
					while (*Right)
					{
						*(Left++) = *(Right++);
					}
				}
			}

			FASTINLINE static void Strncpy(CharType* Left, const CharType* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				if (Left && Right) {
					while (*Right && Max)
					{
						*(Left++) = *(Right++);
						--Max;
					}
				}
			}

			static constexpr bool IsNumber(const CharType Char)
			{ return Consts::Number(0) <= Char && Consts::Number(9) >= Char; }

			static constexpr bool IsWhitespace(const CharType Char)
			{ return Consts::Space == Char || Consts::Tab == Char; }

			static constexpr bool IsAlphabet(const CharType Char) {
				return (Consts::Alphabet(0) <= Char && Consts::Alphabet(25) >= Char) ||
					(Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(25) >= Char);
			}
		};

		/* MBS implementation. */
		template<> struct TNativeStrings<ansi_t, false> {
			FASTINLINE static size_t Strlen(const c8* String) { return ::strlen(String); }
			FASTINLINE static s32 Strcmp(const c8* Left, const c8* Right) { return ::strcmp(Left, Right); }
			FASTINLINE static s32 Strncmp(const c8* Left, const c8* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				return ::strncmp(Left, Right, Max);
			}

			FASTINLINE static void Strcpy(c8* Left, const c8* Right) { ::strcpy(Left, Right); }
			FASTINLINE static void Strncpy(c8* Left, const c8* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max) 
			{
				::strncpy(Left, Right, Max);
			}

			static constexpr bool IsNumber(const c8 Char) {
				return Char >= '0' && Char <= '9';
			}

			static constexpr bool IsWhitespace(const c8 Char) {
				return Char == ' ' || Char == '\t';
			}

			static constexpr bool IsAlphabet(const c8 Char) {
				return (Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z');
			}
		};

		/* WCS implementation. */
#if PLATFORM_NATIVE_WCHAR
		template<> struct TNativeStrings<wide_t, false> {
			FASTINLINE static size_t Strlen(const wide_t* String) { return ::wcslen(String); }
			FASTINLINE static s32 Strcmp(const wide_t* Left, const wide_t* Right) { return ::wcscmp(Left, Right); }
			FASTINLINE static s32 Strncmp(const wide_t* Left, const wide_t* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max) 
			{
				return ::wcsncmp(Left, Right, Max);
			}


			FASTINLINE static void Strcpy(wide_t* Left, const wide_t* Right) { ::wcscpy(Left, Right); }
			FASTINLINE static void Strncpy(wide_t* Left, const wide_t* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				::wcsncpy(Left, Right, Max);
			}

			static constexpr bool IsNumber(const wide_t Char) {
				return Char >= L'0' && Char <= L'9';
			}

			static constexpr bool IsWhitespace(const wide_t Char) {
				return Char == L' ' || Char == L'\t';
			}

			static constexpr bool IsAlphabet(const wide_t Char) {
				return (Char >= L'a' && Char <= L'z') || (Char >= L'A' && Char <= L'Z');
			}
		};
#endif
		template<typename CharType>
		struct TNativeStrings<CharType, true>
		{
			using NumType = typename CharType::NumType;
			using Forwards = TNativeStrings<typename CharType::NumType, false>;

			FASTINLINE static size_t Strlen(const CharType* String) {
				return Forwards::Strlen((const NumType*)String);
			}

			FASTINLINE static s32 Strcmp(const CharType* Left, const CharType* Right) {
				return Forwards::Strcmp((const NumType*)Left, (const NumType*)Right);
			}

			FASTINLINE static s32 Strncmp(const CharType* Left, const CharType* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max) 
			{
				return Forwards::Strncmp((const NumType*)Left, (const NumType*)Right, Max);
			}

			FASTINLINE static void Strcpy(CharType* Left, const CharType* Right) {
				Forwards::Strcpy((NumType*)Left, (const NumType*)Right);
			}

			FASTINLINE static void Strncpy(CharType* Left, const CharType* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				Forwards::Strncpy((NumType*)Left, (const NumType*)Right, Max);
			}

			static constexpr bool IsNumber(const CharType Char) {
				return Forwards::IsNumber(Char._);
			}

			static constexpr bool IsWhitespace(const CharType Char) {
				return Forwards::IsWhitespace(Char._);
			}

			static constexpr bool IsAlphabet(const CharType Char) {
				return Forwards::IsAlphabet(Char._);
			}
		};

		enum class EIconvEnc
		{
			WideType,
			UtfType
		};

		struct FStringConverter_Iconv
		{
#if PLATFORM_POSIX 
			BOXPP static ssize_t Measure(
				EIconvEnc DestEnc, EIconvEnc SrcEnc,
				const void* Src, ssize_t Max
			);

			BOXPP static ssize_t Convert(
				EIconvEnc DestEnc, EIconvEnc SrcEnc,
				void* Dest, const void* Src, ssize_t Max
			);
#else
			FASTINLINE static ssize_t Measure(
				EIconvEnc DestEnc, EIconvEnc SrcEnc,
				const void* Src, ssize_t Max
			) {
				return -1;
			}

			FASTINLINE static ssize_t Convert(
				EIconvEnc DestEnc, EIconvEnc SrcEnc,
				void* Dest, const void* Src, ssize_t Max
			) {
				return -1;
			}
#endif
		};
	}

	/* Native string functions */
	template<typename CharType>
	struct TNativeStrings
		:	public _::TNativeStringConstants<CharType>,
			public _::TNativeStrings<CharType>
	{
	};
}

#endif