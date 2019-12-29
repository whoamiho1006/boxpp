#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/Constants.hpp>
#include <boxpp/base/tpls/strings/Operations.hpp>

#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/opacities/posix.hpp>

#include <boxpp/base/tpls/containers/Array.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace boxpp
{
	namespace strings {
		template<typename DestType, typename SrcType>
		struct TConverter
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const SrcType* Src) { return -1; }

			/* Convert SRC to DEST. */
			FASTINLINE static ssize_t Convert(DestType* Dst, const SrcType* Src) { return -1; }
		};

		template<typename Type>
		struct TConverter<Type, Type>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const Type* Src) {
				return TOperations<Type>::Strlen(Src);
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

#if PLATFORM_NATIVE_WCHAR
		template<>
		struct TConverter<ansi_t, wide_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const wide_t* Src) {
#if PLATFORM_WINDOWS
				size_t V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, nullptr, 0,
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
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, nullptr, 0,
					nullptr, nullptr);

				size_t V = w32_compat::WideCharToMultiByte(
					w32_compat::COMPAT_CP_ACP, 0, Src,
					w32_compat::INT(TOperations<wide_t>::Strlen(Src)),
					Dst, w32_compat::INT(R), nullptr, nullptr);
#endif
#if PLATFORM_POSIX
				size_t V = wcstombs(Dst, Src, TOperations<wide_t>::Strlen(Src));
#endif

				return V;
			}
		};

		template<>
		struct TConverter<wide_t, ansi_t>
		{
			/* Measure length of final string. */
			FASTINLINE static ssize_t Measure(const ansi_t* Src) {
#if PLATFORM_WINDOWS
				size_t V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, nullptr, 0);
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
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, nullptr, 0);

				size_t V = w32_compat::MultiByteToWideChar(
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, Dst, w32_compat::INT(R));
#endif
#if PLATFORM_POSIX
				size_t V = mbstowcs(nullptr, Src, 0);
#endif
				return V;
			}
		};

		/*
			TODO: UTF Series supports.
		*/
#endif

	}

	/* Native string converters. */
	template<typename DestType, typename SrcType>
	class TStringConverter
	{
		using Converter = strings::TConverter<DestType, SrcType>;

	public:
		TStringConverter()
			: Converted(nullptr), ConvertedLength(0),
			  CacheKey(nullptr), CacheKeySz(-1)
		{
		}

		TStringConverter(const SrcType* Src, ssize_t Srclen = -1)
			: Converted(nullptr), ConvertedLength(0),
			  CacheKey(nullptr), CacheKeySz(-1)
		{
			Reset(Src, Srclen);
		}

		~TStringConverter()
		{
			if (Converted)
			{
				delete[](Converted);
			}

			Converted = nullptr;
			ConvertedLength = 0;
		}

	public:
		FASTINLINE void Reset(const SrcType* Src, ssize_t Srclen = -1) {
			if (CacheKey != Src ||
				CacheKeySz != Srclen)
			{
				if (Converted)
				{
					delete[](Converted);

					Converted = nullptr;
					ConvertedLength = 0;

					CacheKey = nullptr;
					CacheKeySz = -1;
				}

				if (Srclen < 0) {
					FillConv(Src);
				}

				else if (Srclen > 0) {
					TArray<SrcType> Buffer;

					Buffer.Add(0, Srclen + 1);
					memcpy(Buffer.GetRaw(), Src, Srclen * sizeof(SrcType));

					FillConv(Buffer.GetRaw());
				}

				if (Converted) {
					CacheKey = Src;
					CacheKeySz = Srclen;
				}
			}
		}

	private:
		FASTINLINE void FillConv(const SrcType* Src) {
			if (Src) {
				ssize_t Length = Converter::Measure(Src);

				if (Length > 0) {
					Converted = new DestType[Length + 1];
					Length = Converter::Convert(Converted, Src);

					if (Length <= 0) {
						delete[](Converted);
						ConvertedLength = 0;
						Converted = nullptr;
					}

					else {
						Converted[Length] = strings::TConstants<DestType>::Null;
						ConvertedLength = size_t(Length);
					}
				}
			}
		}

	private:
		DestType* Converted;
		size_t ConvertedLength;

		const SrcType* CacheKey;
		ssize_t CacheKeySz;

	public:
		FASTINLINE const DestType* GetConvertedString() const { return Converted; }
		FASTINLINE const size_t GetConvertedLength() const { return ConvertedLength; }

	public:
		FASTINLINE operator bool() const { return Converted; }
		FASTINLINE bool operator !() const { return !Converted; }
		FASTINLINE operator const DestType*() const { return GetConvertedString(); }
	};
}