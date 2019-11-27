#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/Constants.hpp>
#include <boxpp/base/tpls/strings/Operations.hpp>

#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/opacities/posix.hpp>

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
					TOperations<wide_t>::Strlen(Src),
					Dst, R, nullptr, nullptr);
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
					w32_compat::COMPAT_CP_ACP, 0, Src, -1, Dst, R);
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
		TStringConverter(const SrcType* Src)
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
						Converted[Length] = strings::TConstants<DestType>::Null;
						ConvertedLength = size_t(Length);
					}
				}
			}
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