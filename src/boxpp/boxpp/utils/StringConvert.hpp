#ifndef __BOXPP_STRINGCONVERT_HPP__
#define __BOXPP_STRINGCONVERT_HPP__
#include <boxpp/utils/NativeStrings.hpp>
#include <boxpp/hints/windows.hpp>

namespace boxpp
{
	namespace _ {
		template<typename DestType, typename SrcType>
		struct TInternalStringConverter
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const SrcType* Src) { return -1; }

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(DestType* Dst, const SrcType* Src) { return -1; }
		};

		template<typename Type>
		struct TInternalStringConverter<Type, Type>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const Type* Src) {
				return TNativeStrings<Type>::Strlen(Src);
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(Type* Dst, const Type* Src) {
				if (Dst && Src) {
					ssize_t Len = 0;

					while (*Src)
					{
						*(Dst++) = *(Src++);
						++Len;
					}

					return Len;
				}

				return 0;
			}
		};

		template<> 
		struct TInternalStringConverter<ansi_t, wide_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const wide_t* Src) {
#if PLATFORM_WINDOWS
				size_t V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCWSTR) Src, -1, nullptr, 0,
					nullptr, nullptr);
#endif
#if PLATFORM_POSIX
				size_t V = wcstombs(nullptr, Src, 0);
#endif

				return V;
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(ansi_t* Dst, const wide_t* Src) {
#if PLATFORM_WINDOWS
				size_t R = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCWSTR) Src, -1, nullptr, 0,
					nullptr, nullptr);

				size_t V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCWSTR) Src,
					_::TNativeStrings<wide_t>::Strlen(Src),
					(w32_compat::LPSTR)Dst, R, nullptr, nullptr);
#endif
#if PLATFORM_POSIX
				size_t V = wcstombs(Dst, Src, _::TNativeStrings<wide_t>::Strlen(Src));
#endif

				return V;
			}
		};

		template<>
		struct TInternalStringConverter<wide_t, ansi_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const ansi_t* Src) {
#if PLATFORM_WINDOWS
				size_t V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCSTR) Src, -1, nullptr, 0);
#endif
#if PLATFORM_POSIX
				size_t V = mbstowcs(nullptr, Src, 0);
#endif
				return V;
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(wide_t* Dst, const ansi_t* Src) {
#if PLATFORM_WINDOWS
				size_t R = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCSTR) Src, -1, nullptr, 0);

				size_t V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_ACP, 0, (w32_compat::LPCSTR) Src, -1,
					(w32_compat::LPCWSTR)Dst, R);
#endif
#if PLATFORM_POSIX
				size_t V = mbstowcs(nullptr, Src, 0);
#endif
				return V;
			}
		};

		/*
			WCHAR to UTF-8.
		*/
		template<>
		struct TInternalStringConverter<utf8_t, wide_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const wide_t* Src)
			{
				ssize_t V = -1;
				
#if PLATFORM_WINDOWS
				V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCWSTR) Src, -1, nullptr, 0,
					nullptr, nullptr);
#endif
#if PLATFORM_POSIX
				V = FStringConverter_Iconv::Measure(EIconvEnc::UtfType, EIconvEnc::WideType, Src, 
					_::TNativeStrings<wide_t>::Strlen(Src));
#endif

				return V;
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(utf8_t* Dst, const wide_t* Src)
			{
				ssize_t V = -1;

#if PLATFORM_WINDOWS
				size_t R = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCWSTR) Src, -1, nullptr, 0,
					nullptr, nullptr);

				V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCWSTR) Src,
					_::TNativeStrings<wide_t>::Strlen(Src),
					(w32_compat::LPSTR)Dst, R, nullptr, nullptr);
#endif
#if PLATFORM_POSIX
				V = FStringConverter_Iconv::Measure(EIconvEnc::UtfType, EIconvEnc::WideType, Src, 
					_::TNativeStrings<wide_t>::Strlen(Src));
#endif

				return V;
			}

		};

		/*
			UTF-8 To WCHAR.
		*/
		template<>
		struct TInternalStringConverter<wide_t, utf8_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const utf8_t* Src)
			{
				ssize_t V = -1;

#if PLATFORM_WINDOWS
				V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCSTR) Src, -1, nullptr, 0);
#endif
#if PLATFORM_POSIX
				V = FStringConverter_Iconv::Measure(EIconvEnc::UtfType, EIconvEnc::WideType, Src, 
					_::TNativeStrings<utf8_t>::Strlen(Src));
#endif

				return V;
			}

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(wide_t* Dst, const utf8_t* Src)
			{
				ssize_t V = -1;

#if PLATFORM_WINDOWS
				size_t R = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCSTR) Src, -1, nullptr, 0);

				V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_UTF8, 0, (w32_compat::LPCSTR) Src, -1,
					(w32_compat::LPCWSTR)Dst, R);
#endif
#if PLATFORM_POSIX
				V = FStringConverter_Iconv::Measure(EIconvEnc::UtfType, EIconvEnc::WideType, Src, 
					_::TNativeStrings<utf8_t>::Strlen(Src));
#endif

				return V;
			}
		};
		/*
		-------------------------------------------------
			TODO: conversion between utf series
		-------------------------------------------------
							2 bytes wchar_t		4 bytes wchar_t
		utf-8 to utf-16		OK
		utf-8 to utf-32							OK

		utf-16 to utf-8		OK
		utf-16 to utf-32

		utf-32 to utf-8							OK
		utf-32 to utf-16
		*/

#define BOXPP_STRINGCONVERT_FORWARD_TO(DestType, SrcType, FwdDest, FwdSrc) \
	template<> struct TInternalStringConverter<DestType, SrcType> { \
		FASTINLINE static ssize_t Measure(const utf8_t* Src) { return TInternalStringConverter<FwdDest, FwdSrc>::Measure((const FwdSrc*) Src); } \
		FASTINLINE static ssize_t Convert(wide_t* Dst, const utf8_t* Src) { return TInternalStringConverter<FwdDest, FwdSrc>::Convert((FwdDest*) Dst, (const FwdSrc*) Src); } \
	}

#if __SIZEOF_WCHAR_T__ == 2
		BOXPP_STRINGCONVERT_FORWARD_TO(utf8_t, utf16_t, utf8_t, wide_t);
		BOXPP_STRINGCONVERT_FORWARD_TO(utf16_t, utf8_t, wide_t, utf8_t);

		BOXPP_STRINGCONVERT_FORWARD_TO(c32, c8, utf32_t, utf8_t);
		BOXPP_STRINGCONVERT_FORWARD_TO(c8, c32, utf8_t, utf32_t);
#endif
#if __SIZEOF_WCHAR_T__ == 4
		BOXPP_STRINGCONVERT_FORWARD_TO(utf8_t, utf32_t, utf8_t, wide_t);
		BOXPP_STRINGCONVERT_FORWARD_TO(utf32_t, utf8_t, wide_t, utf8_t);

		BOXPP_STRINGCONVERT_FORWARD_TO(c16, c8, utf32_t, utf8_t);
		BOXPP_STRINGCONVERT_FORWARD_TO(c8, c16, utf8_t, utf32_t);
#endif
		BOXPP_STRINGCONVERT_FORWARD_TO(c16, c32, utf32_t, utf16_t);
		BOXPP_STRINGCONVERT_FORWARD_TO(c32, c16, utf16_t, utf32_t);

	}


	/* Native string converters. */
	template<typename DestType, typename SrcType>
	class TStringConvert
	{
		using Converter = _::TInternalStringConverter<DestType, SrcType>;

	public:
		TStringConvert(const SrcType* Src)
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
						Converted[Length] = _::TNativeStringConstants<DestType>::Null;
						ConvertedLength = size_t(Length);
					}
				}
			}
		}

		~TStringConvert()
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

#endif // !__BOXPP_STRINGCONVERT_HPP__