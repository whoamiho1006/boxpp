#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/Constants.hpp>
#include <boxpp/base/tpls/traits/Movable.hpp>

#include <string.h>
#include <stdlib.h>
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

			FASTINLINE static f64 Strtod(const CharType* String);

			FASTINLINE static f32 Strtof(const CharType* String) { return (f32)Strtod(String); }

			FASTINLINE static f64 Strtod(const CharType* String, CharType*& StopedAt);
			FASTINLINE static f32 Strtof(const CharType* String, CharType*& StopedAt) { return (f32)Strtod(String, StopedAt); }
		};

		template<typename CharType>
		struct TCommonOperations
		{
			using Consts = TConstants<CharType>;

			static constexpr bool IsDigit(const CharType Char)
			{
				return (Consts::Number(0) <= Char && Consts::Number(9) >= Char) ||
					(Consts::Alphabet(0) <= Char && Consts::Alphabet(5) >= Char) ||
					(Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(5) >= Char);
			}

			static constexpr s32 HexVal(const CharType Char)
			{
				if (Consts::Number(0) <= Char && Consts::Number(9) >= Char)
					return Char - Consts::Number(0);

				else if (Consts::Alphabet(0) <= Char && Consts::Alphabet(5) >= Char)
					return Char - Consts::Alphabet(0) + 10;

				else if (Consts::AlphabetUpper(0) <= Char && Consts::AlphabetUpper(5) >= Char)
					return Char - Consts::AlphabetUpper(0) + 10;

				return 0;
			}

			static constexpr s32 NumVal(const CharType Char)
			{
				if (Consts::Number(0) <= Char && Consts::Number(9) >= Char)
					return Char - Consts::Number(0);

				return 0;
			}

			FASTINLINE static ssize_t OffsetOf(const CharType* Haystack, const CharType& Char) {
				ssize_t Offset = -1;

				if (Haystack) {
					Offset = 0;

					while (*Haystack && *Haystack != Char) {
						++Haystack; ++Offset;
					}

					return *Haystack == Char ? Offset : -1;
				}

				return Offset;
			}

			FASTINLINE static ssize_t OffsetOf(const CharType* Haystack, const CharType& Char, size_t Haymax) {
				ssize_t Offset = -1; 
				
				if (Haystack) {
					Offset = 0;

					while (*Haystack && *Haystack != Char && Haymax) {
						++Haystack; --Haymax; ++Offset;
					};

					return (Haymax && *Haystack == Char) ? Offset : -1;
				}

				return Offset;
			}

			FASTINLINE static bool Contains(const CharType* Haystack, const CharType& Char) {
				if (Haystack) {
					while (*Haystack && *Haystack != Char) ++Haystack;
					return *Haystack == Char;
				}

				return false;
			}

			FASTINLINE static bool Contains(const CharType* Haystack, const CharType& Char, size_t Haymax) {
				if (Haystack) {
					while (*Haystack && *Haystack != Char && Haymax) { ++Haystack; --Haymax; };
					return Haymax && *Haystack == Char;
				}

				return false;
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

			FASTINLINE static s64 Atol(const CharType* Char, ssize_t Radix = -1) {
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
					if (Radix != 8 &&
						Char[0] == Consts::Hipen)
					{
						Neg = -1; ++Char;
					}

					if (Radix < 0)
						Radix = 10;

					if (Radix != 16) {
						while (*Char && (Consts::Number(0) <= *Char && Consts::Number(9) >= *Char))
							Value = Value * Radix + NumVal(*Char++);
					}

					else {
						while (*Char && IsDigit(*Char))
							Value = Value * 16 + HexVal(*Char++);
					}
				}

				return Value * Neg;
			}

			FASTINLINE static u64 Atoul(const CharType* Char, ssize_t Radix = -1)
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
					if (Radix < 0)
						Radix = 10;
					
					if (Radix != 16) {
						while (*Char && (Consts::Number(0) <= *Char && Consts::Number(9) >= *Char))
							Value = Value * Radix + NumVal(*Char++);
					}

					else {
						while (*Char && IsDigit(*Char))
							Value = Value * 16 + HexVal(*Char++);
					}
				}

				return Value;
			}

			FASTINLINE static f64 Atod(const CharType* String) {
				f64 RetVal = 0.0;
				s32 Exponent = 0;
				CharType Temp;
				
				while ((Temp = *String++) != Consts::Null && 
					(Consts::Number(0) <= Temp && Consts::Number(9) >= Temp)) 
				{
					RetVal = RetVal * 10.0 + (Temp - Consts::Number(0));
				}

				if (Temp == Consts::Dot) {
					while ((Temp = *String++) != Consts::Null && 
						(Consts::Number(0) <= Temp && Consts::Number(9) >= Temp))
					{
						RetVal = RetVal * 10.0 + (Temp - Consts::Number(0));
						Exponent = Exponent - 1;
					}
				}

				if (Temp == Consts::Alphabet(4) || 
					Temp == Consts::AlphabetUpper(4))
				{
					s32 Sign = 1;
					s32 i = 0;

					Temp = *String++;
					if (Temp == Consts::Plus || Temp == Consts::Hipen) {
						Sign = (Temp == Consts::Hipen ? -1 : 1);
						Temp = *String++;
					}

					while (IsDigit(Temp)) {
						i = i * 10 + (Temp - Consts::Number(0));
						Temp = *String++;
					}

					Exponent += i * Sign;
				}

				while (Exponent > 0) {
					RetVal *= 10.0;
					Exponent--;
				}

				while (Exponent < 0) {
					RetVal *= 0.1;
					Exponent++;
				}

				return RetVal;
			}

			FASTINLINE static CharType AlphabetOffset(const CharType& InChar) {
				if (InChar >= Consts::Alphabet(0) &&
					InChar <= Consts::Alphabet(24)) 
				{
					return InChar - Consts::Alphabet(0);
				}

				else if (InChar >= Consts::AlphabetUpper(0) && 
						 InChar <= Consts::AlphabetUpper(24)) 
				{
					return InChar - Consts::AlphabetUpper(0);
				}


				return InChar;
			}

			/*
				compare two string and returns its difference. (case insensitive)
			*/
			FASTINLINE static s32 StrcmpCI(const CharType* Left, const CharType* Right) {
				if (Left && Right)
				{
					while (*Left && *Right &&
						!(AlphabetOffset(*Left) - AlphabetOffset(*Right)))
					{
						++Left; ++Right;
					}

					if (((*Left >= Consts::Alphabet(0) && *Left <= Consts::Alphabet(24)) || (
						  *Left >= Consts::AlphabetUpper(0) && *Left <= Consts::AlphabetUpper(24))) &&
						((*Right >= Consts::Alphabet(0) && *Right <= Consts::Alphabet(24)) || (
						  *Right >= Consts::AlphabetUpper(0) && *Right <= Consts::AlphabetUpper(24))))
					{
						return AlphabetOffset(*Left) - AlphabetOffset(*Right);
					}

					return *Left - *Right;
				}

				return Left == Right ? 0 : (Left ? 1 : -1);
			}

			/*
				compare two string and returns its difference. (case insensitive)
			*/
			FASTINLINE static s32 StrncmpCI(const CharType* Left, const CharType* Right, size_t SizeMax) {
				if (Left && Right && SizeMax)
				{
					while (*Left && *Right && SizeMax &&
						!(AlphabetOffset(*Left) - AlphabetOffset(*Right)))
					{
						++Left; ++Right;
						--SizeMax;
					}

					if (SizeMax) {
						if (((*Left >= Consts::Alphabet(0) && *Left <= Consts::Alphabet(24)) || (
							  *Left >= Consts::AlphabetUpper(0) && *Left <= Consts::AlphabetUpper(24))) &&
							((*Right >= Consts::Alphabet(0) && *Right <= Consts::Alphabet(24)) || (
							  *Right >= Consts::AlphabetUpper(0) && *Right <= Consts::AlphabetUpper(24))))
						{
							return AlphabetOffset(*Left) - AlphabetOffset(*Right);
						}
					}

					return !SizeMax ? 0 : *Left - *Right;
				}

				return !SizeMax || Left == Right ? 0 : (Left ? 1 : -1);
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

			FASTINLINE static f64 Strtod(const ansi_t* String) { return strtod(String, nullptr); }
			FASTINLINE static f32 Strtof(const ansi_t* String) { return strtof(String, nullptr); }

			FASTINLINE static f64 Strtod(const ansi_t* String, ansi_t*& StopedAt) { return strtod(String, &StopedAt); }
			FASTINLINE static f32 Strtof(const ansi_t* String, ansi_t*& StopedAt) { return strtof(String, &StopedAt); }
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

			FASTINLINE static f64 Strtod(const wide_t* String) { return wcstod(String, nullptr); }
			FASTINLINE static f32 Strtof(const wide_t* String) { return wcstof(String, nullptr); }

			FASTINLINE static f64 Strtod(const wide_t* String, wide_t*& StopedAt) { return wcstod(String, &StopedAt); }
			FASTINLINE static f32 Strtof(const wide_t* String, wide_t*& StopedAt) { return wcstof(String, &StopedAt); }
		};
#endif
#endif

		template<typename CharType>
		FASTINLINE f64 TOperations<CharType>::Strtod(const CharType* String) {
			using Common = TCommonOperations<CharType>;
			return Common::Atod(String);
		}

		template<typename CharType>
		FASTINLINE static f64 TOperations<CharType>::Strtod(const CharType* String, CharType*& StopedAt) {
			using Common = TCommonOperations<CharType>;
			CharType* Cursor = (CharType*)String;

			if (*Cursor == '+' || *Cursor == '-')
				++Cursor;

			/* INF or INFINITY.  */
			if ((Cursor[0] == 'i' || Cursor[0] == 'I')
				&& (Cursor[1] == 'n' || Cursor[1] == 'N')
				&& (Cursor[2] == 'f' || Cursor[2] == 'F'))
			{
				if ((Cursor[3] == 'i' || Cursor[3] == 'I')
					&& (Cursor[4] == 'n' || Cursor[4] == 'N')
					&& (Cursor[5] == 'i' || Cursor[5] == 'I')
					&& (Cursor[6] == 't' || Cursor[6] == 'T')
					&& (Cursor[7] == 'y' || Cursor[7] == 'Y'))
				{
					StopedAt = Cursor + 8;
					return Common::Atod(String);
				}
				else
				{
					StopedAt = Cursor + 3;
					return Common::Atod(String);
				}
			}
			/* NAN or NAN(foo).  */
			if ((Cursor[0] == 'n' || Cursor[0] == 'N')
				&& (Cursor[1] == 'a' || Cursor[1] == 'A')
				&& (Cursor[2] == 'n' || Cursor[2] == 'N'))
			{
				Cursor += 3;
				if (*Cursor == '(')
				{
					++Cursor;
					while (*Cursor != '\0' && *Cursor != ')')
						++Cursor;
					if (*Cursor == ')')
						++Cursor;
				}

				StopedAt = Cursor;
				return Common::Atod(String);
			}

			/* digits, with 0 or 1 periods in it.  */
			if (Common::IsDigit(*Cursor) || *Cursor == '.')
			{
				bool HasDot = false;
				while (Common::IsDigit(*Cursor) || (!HasDot && *Cursor == '.'))
				{
					if (*Cursor == '.')
						HasDot = true;

					++Cursor;
				}

				/* Exponent.  */
				if (*Cursor == 'e' || *Cursor == 'E')
				{
					int i = 1;

					if (Cursor[i] == '+' || Cursor[i] == '-')
						++i;

					if (Common::IsDigit(Cursor[i]))
					{
						while (Common::IsDigit(Cursor[i]))
							++i;

						StopedAt = Cursor + i;
						return Common::Atod(String);
					}
				}

				StopedAt = Cursor;
				return Common::Atod(String);
			}

			/* Didn't find any digits.  Doesn't look like a number.  */
			StopedAt = (CharType*)String;
			return 0.0;
		}
	}
}