#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/Constants.hpp>

#include <string.h>
#if PLATFORM_NATIVE_WCHAR
#	include <wchar.h>
#endif

namespace boxpp
{
	namespace strings {

		template<typename CharType>
		struct TOperations
		{
			using Consts = TConstants<CharType>;

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
			{
				return Consts::Number(0) <= Char && Consts::Number(9) >= Char;
			}

			static constexpr bool IsWhitespace(const CharType Char)
			{
				return Consts::Space == Char || Consts::Tab == Char;
			}

			static constexpr bool IsAlphabet(const CharType Char) {
				return (Consts::Alphabet(0) <= Char && Consts::Alphabet(24) >= Char) ||
					(Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(24) >= Char);
			}

		};

		template<typename CharType>
		struct TCommonOperations
		{
			using Consts = TConstants<CharType>;
			using Ops = TOperations<CharType>;

			static constexpr bool IsDigit(const CharType Char)
			{
				return (Consts::Number(0) <= Char && Consts::Number(9) >= Char) ||
					(Consts::Alphabet(0) <= Char && Consts::Alphabet(5) >= Char) ||
					(Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(5) >= Char);
			}

			static constexpr s32 HexVal(const CharType Char)
			{
				if (Ops::IsNumber(Char))
					return Char - Consts::Number(0);

				else if (Consts::Alphabet(0) <= Char && Consts::Alphabet(5) >= Char)
					return Char - Consts::Alphabet(0) + 10;

				else if (Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(5) >= Char)
					return Char - Consts::AlphabetUpper(0) + 10;

				return 0;
			}

			static constexpr s32 NumVal(const CharType Char)
			{
				if (Ops::IsNumber(Char))
					return Char - Consts::Number(0);

				return 0;
			}

			FASTINLINE static bool Ltoa(s64 Val, CharType* Buffer, u32 Radix = 10, ssize_t MaxSize = -1) {
				if (CharType* Current = Buffer) {
					bool Neg = Val < 0;
					size_t Len = 0;
					u64 UVal = 0;

					if (Radix == 10) {
						Val = Neg ? -Val : Val;

						if (Neg && (MaxSize < 0 || MaxSize)) {
							*(Current++) = Consts::Hipen;
							--MaxSize; ++Buffer;
						}
					}

					UVal = *((u64*)&Val);
					while (MaxSize < 0 || MaxSize) {
						u32 Digit = UVal % Radix;
						UVal = UVal / Radix;

						if (Digit < 10) {
							*(Current++) = Consts::Number(Digit);
							--MaxSize;
						}
						else {
							*(Current++) = Consts::Alphabet(Digit - 10);
							--MaxSize;
						}

						++Len;

						if (!UVal) {
							break;
						}
					}

					for (size_t i = 0; i < Len / 2; ++i) {
						Swap(Buffer[i], Buffer[Len - i - 1]);
					}

					if (MaxSize < 0 || MaxSize) {
						*(Current++) = 0;
					}

					return true;
				}

				return false;
			}

			FASTINLINE static bool Ultoa(u64 Val, CharType* Buffer, u32 Radix = 10, ssize_t MaxSize = -1) {
				if (CharType* Current = Buffer) {
					bool Neg = Val < 0;
					size_t Len = 0;

					while (MaxSize < 0 || MaxSize) {
						u32 Digit = Val % Radix;
						Val = Val / Radix;

						if (Digit < 10) {
							*(Current++) = Consts::Number(Digit);
							--MaxSize;
						}
						else {
							*(Current++) = Consts::Alphabet(Digit - 10);
							--MaxSize;
						}

						++Len;

						if (!Val) {
							break;
						}
					}

					for (size_t i = 0; i < Len / 2; ++i) {
						Swap(Buffer[i], Buffer[Len - i - 1]);
					}

					if (MaxSize < 0 || MaxSize) {
						*(Current++) = 0;
					}

					return true;
				}

				return false;
			}

			FASTINLINE static s64 Atol(const CharType* Char)
			{
				s64 Value = 0;
				s32 Neg = 1;

				// Hex: 0x...
				if (Char[0] == Consts::Number(0) &&
					Char[1] == Consts::Alphabet(22))
				{
					Char += 2;

					while (*Char && IsDigit(*Char))
						Value = Value * 16 + HexVal(*Char++);
				}

				// Dec: ...
				else
				{
					if (Char[0] == Consts::Hipen) {
						Neg = -1; ++Char;
					}

					while (*Char && Ops::IsNumber(*Char))
						Value = Value * 10 + NumVal(*Char++);
				}

				return Value * Neg;
			}

			FASTINLINE static u64 Atoul(const CharType* Char)
			{
				using Consts = TConstants<CharType>;

				u64 Value = 0;

				// Hex: 0x...
				if (Char[0] == Consts::Number(0) &&
					Char[1] == Consts::Alphabet(22))
				{
					Char += 2;

					while (*Char && IsDigit(*Char))
						Value = Value * 16 + HexVal(*Char++);
				}

				// Dec: ...
				else
				{
					while (*Char &&Ops::IsNumber(*Char))
						Value = Value * 10 + NumVal(*Char++);
				}

				return Value;
			}
		};

#if defined(PLATFORM_USE_STRING_BY_CRT) && PLATFORM_USE_STRING_BY_CRT
		/* MBS implementation. */
		template<> struct TOperations<ansi_t> {
			using Consts = TConstants<ansi_t>;
			FASTINLINE static size_t Strlen(const ansi_t* String) { return ::strlen(String); }
			FASTINLINE static s32 Strcmp(const ansi_t* Left, const ansi_t* Right) { return ::strcmp(Left, Right); }
			FASTINLINE static s32 Strncmp(const ansi_t* Left, const ansi_t* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				return ::strncmp(Left, Right, Max);
			}

			FASTINLINE static void Strcpy(ansi_t* Left, const ansi_t* Right) { ::strcpy(Left, Right); }
			FASTINLINE static void Strncpy(ansi_t* Left, const ansi_t* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				::strncpy(Left, Right, Max);
			}

			static constexpr bool IsNumber(const ansi_t Char) {
				return Char >= '0' && Char <= '9';
			}

			static constexpr bool IsWhitespace(const ansi_t Char) {
				return Char == ' ' || Char == '\t';
			}

			static constexpr bool IsAlphabet(const ansi_t Char) {
				return (Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z');
			}
		};

		/* WCS implementation. */
#if PLATFORM_NATIVE_WCHAR
		template<> struct TOperations<wide_t> {
			using Consts = TConstants<wide_t>;

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
#endif
	}
}