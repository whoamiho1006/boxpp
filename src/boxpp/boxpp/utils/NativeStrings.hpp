#ifndef __BOXPP_NATIVESTRINGS_HPP__
#define __BOXPP_NATIVESTRINGS_HPP__

#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

namespace boxpp
{
	namespace _ {
		/* All C* Series have same code position for bellow characters. */
		template<typename CharType>
		struct TStandardStringConstants {
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

		/* All UTF* Series have same code position for bellow characters. */
#define BOXPP_UTF_STRING_CONSTANTS(utfn_t, cn_t) \
		template<> struct TStandardStringConstants<utfn_t> {	\
			static constexpr const utfn_t Space = { (cn_t)' ' };	\
			static constexpr const utfn_t Tab = { (cn_t)'\t' };	\
			static constexpr const utfn_t Backspace = { (cn_t)'\b' };	\
			static constexpr const utfn_t LineFeed = { (cn_t)'\n' };	\
			static constexpr const utfn_t CarrageReturn = { (cn_t)'\r' };	\
			static constexpr const utfn_t Asteroid = { (cn_t)'*' };	\
			static constexpr const utfn_t Slash = { (cn_t)'/' };	\
			static constexpr const utfn_t Backslash = { (cn_t)'\\' };	\
			\
			static constexpr const utfn_t Hipen = { (cn_t)'-' };	\
			static constexpr const utfn_t Underbar = { (cn_t)'_' };	\
			static constexpr const utfn_t At = {(cn_t) '@' };	\
			static constexpr const utfn_t Sharp = { (cn_t)'#' };	\
			static constexpr const utfn_t Question = {(cn_t) '?' };	\
			static constexpr const utfn_t Less = {(cn_t) '<' };	\
			static constexpr const utfn_t Greater = {(cn_t) '>' };	\
			\
			static constexpr const utfn_t Collon = {(cn_t) ':' };	\
			static constexpr const utfn_t Semicollon = {(cn_t) ';' };	\
			\
			static constexpr const utfn_t Alphabet(size_t Offset) { return { (cn_t)("abcdefghijklmnopqrstuvwxyz"[Offset]) }; }		\
			static constexpr const utfn_t AlphabetUpper(size_t Offset) { return { (cn_t)("ABCDEFGHIJKLMNOPQRSTUVWXYZ"[Offset]) }; }	\
			static constexpr const utfn_t Number(size_t Offset) { return { (cn_t)("0123456789"[Offset]) }; }	\
		}

		BOXPP_UTF_STRING_CONSTANTS(utf8_t, c8);
		BOXPP_UTF_STRING_CONSTANTS(utf16_t, c16);
		BOXPP_UTF_STRING_CONSTANTS(utf32_t, c32);
				
		/* Common implementation. */
		template<typename CharType>
		struct TStandardStrings
		{
			using Consts = TStandardStringConstants<CharType>;

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
					while (*Left)
					{
						*(Left++) = *(Right++);
					}
				}
			}

			FASTINLINE static void Strncpy(CharType* Left, const CharType* Right,
				size_t Max = type_db::TNumberLimits<size_t>::Max)
			{
				if (Left && Right) {
					while (*Left && Max)
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
		template<> struct TStandardStrings<c8> {
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
		template<> struct TStandardStrings<wide_t> {
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

#define BOXPP_UTF_STANDARD_STRINGS(utfn_t, cn_t) \
		template<> struct TStandardStrings<utfn_t> { \
			using Org = TStandardStrings<cn_t>; \
			FASTINLINE static size_t Strlen(const utfn_t* String) { return Org::Strlen((const cn_t*) String); } \
			FASTINLINE static s32 Strcmp(const utfn_t* Left, const utfn_t* Right) { return Org::Strcmp((const cn_t*) Left, (const cn_t*) Right); } \
			FASTINLINE static s32 Strncmp(const utfn_t* Left, const utfn_t* Right, size_t Max = type_db::TNumberLimits<size_t>::Max) \
				{ return Org::Strncmp((const cn_t*) Left, (const cn_t*) Right, Max); } \
			FASTINLINE static void Strcpy(utfn_t* Left, const utfn_t* Right) { return Org::Strcpy((cn_t*) Left, (const cn_t*) Right); } \
			FASTINLINE static void Strncpy(utfn_t* Left, const utfn_t* Right, size_t Max = type_db::TNumberLimits<size_t>::Max) \
				{ Org::Strncpy((cn_t*) Left, (const cn_t*) Right, Max); } \
			static constexpr bool IsNumber(const utfn_t Char) { return Org::IsNumber(Char._); } \
			static constexpr bool IsWhitespace(const utfn_t Char) { return Org::IsWhitespace(Char._); } \
			static constexpr bool IsAlphabet(const utfn_t Char) { return Org::IsAlphabet(Char._); } \
		};

		BOXPP_UTF_STANDARD_STRINGS(utf8_t, c8);
		BOXPP_UTF_STANDARD_STRINGS(utf16_t, c16);
		BOXPP_UTF_STANDARD_STRINGS(utf32_t, c32);

		template<typename DestType, typename SrcType>
		struct TCodecvtStringConvert
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const SrcType* Src, ssize_t Max = -1) { return -1; }

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(DestType* Dst, const SrcType* Src, ssize_t Max = -1) { return -1; }
		};

		template<typename Type>
		struct TCodecvtStringConvert<Type, Type>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const Type* Src, ssize_t Max = -1) {
				ssize_t Size = TStandardStrings<Type>::Strlen(Src);

				if (Max < 0)
					Max = Size;

				if (Size > Max)
					return Max;

				return Size;
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(Type* Dst, const Type* Src, ssize_t Max = -1) {
				if (Max < 0)
				{
					TStandardStrings<Type>::Strcpy(Dst, Src);
					return TStandardStrings<Type>::Strlen(Src);
				}

				if (Left && Right) {
					ssize_t Len = 0;

					while (*Left && Max)
					{
						*(Left++) = *(Right++);
						--Max; ++Len;
					}

					return Len;
				}

				return 0;
			}
		};

		enum class ECodeType {
			C8 = 0, C16, C32,
			UTF8, UTF16, UTF32
		};

		namespace cvts {
			struct BOXPP TCodecvtC2C {
				static ssize_t Measure(ECodeType DestCode, const void* Src, ssize_t Max, ECodeType SrcCode);
				static ssize_t Convert(ECodeType DestCode, void* Dst, const void* Src, ssize_t Max, ECodeType SrcCode);
			};
		}

#define BOXPP_NATIVESTRINGS_FOWARD_IMPL(DestType, SrcType, DestCode, SrcCode) \
		template<> struct TCodecvtStringConvert<DestType, SrcType> { \
			FASTINLINE static ssize_t Measure(const SrcType* Src, ssize_t Max = -1) { return cvts::TCodecvtC2C::Measure(DestCode, Src, Max, SrcCode); } \
			FASTINLINE static ssize_t Convert(DestType* Dst, const SrcType* Src, ssize_t Max = -1) { return cvts::TCodecvtC2C::Convert(DestCode, Dst, Src, Max, SrcCode); } \
		}

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c16, c8, ECodeType::C16, ECodeType::C8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c32, c8, ECodeType::C32, ECodeType::C8);
		
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c8,  c16, ECodeType::C8, ECodeType::C16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c32, c16, ECodeType::C32, ECodeType::C16);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c8,	 c32, ECodeType::C8, ECodeType::C32);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c16, c32, ECodeType::C16, ECodeType::C32);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf8_t, c8, ECodeType::UTF8, ECodeType::C8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf16_t, c8, ECodeType::UTF16, ECodeType::C8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf32_t, c8, ECodeType::UTF32, ECodeType::C8);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c8, utf8_t, ECodeType::C8, ECodeType::UTF8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c8, utf16_t, ECodeType::C8, ECodeType::UTF16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c8, utf32_t, ECodeType::C8, ECodeType::UTF32);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf8_t, c16, ECodeType::UTF8, ECodeType::C16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf16_t, c16, ECodeType::UTF16, ECodeType::C16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf32_t, c16, ECodeType::UTF32, ECodeType::C32);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c16, utf8_t, ECodeType::C16, ECodeType::UTF8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c16, utf16_t, ECodeType::C16, ECodeType::UTF16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c16, utf32_t, ECodeType::C16, ECodeType::UTF32);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf8_t, c32, ECodeType::UTF8, ECodeType::C32);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf16_t, c32, ECodeType::UTF16, ECodeType::C32);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(utf32_t, c32, ECodeType::UTF32, ECodeType::C32);

		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c32, utf8_t, ECodeType::C32, ECodeType::UTF8);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c32, utf16_t, ECodeType::C32, ECodeType::UTF16);
		BOXPP_NATIVESTRINGS_FOWARD_IMPL(c32, utf32_t, ECodeType::C32, ECodeType::UTF32);



	}

	/* Native string functions */
	template<typename CharType>
	struct TNativeStrings
		:	public _::TStandardStringConstants<CharType>,
			public _::TStandardStrings<CharType>
	{
	};

	/* Native string converters. */
	template<typename DestType, typename SrcType>
	class TNativeStringConvert
	{
		using Converter = _::TCodecvtStringConvert<DestType, SrcType>;

	public:
		TNativeStringConvert(const SrcType* Src)
			: Converted(nullptr), ConvertedLength(0)
		{
			if (Src) {
				ssize_t Length = Converter::Measure(Src);

				if (Length > 0) {
					Converted = new DestType[Length + 1];
					Length = Converter::Convert(Converted, Src);

					if (Length < 0) {
						delete[](Converted);
						ConvertedLength = 0;
						Converted = nullptr;
					}

					else {
						ConvertedLength = size_t(Length);
					}
				}
			}
		}

		~TNativeStringConvert()
		{
			if (Converted)
			{
				delete[](Converted);
			}

			Converted = nullptr;
			ConvertedLength = 0;
		}

	private:
		DestType* Converted;
		size_t ConvertedLength;

	public:
		FASTINLINE const DestType* GetConvertedString() const { return Converted; }
		FASTINLINE const size_t GetConvertedLength() const { return ConvertedLength; }

	public:
		FASTINLINE operator DestType*() const { return GetConvertedString(); }
	};
}

#endif