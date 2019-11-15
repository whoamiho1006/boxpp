#include "NativeStrings.hpp"

#if PLATFORM_WINDOWS
#include "../boxpp-iconv/ported-headers/iconv.h"
#endif
#if PLATFORM_POSIX && !BOX_NOT_COMPILED
#include <iconv.h>
#endif

namespace boxpp {
	namespace _ {
		namespace cvts {
			ssize_t TCodecvtC2C::Measure(ECodeType DestCode, const void * Src, ssize_t Max, ECodeType SrcCode)
			{
				/*
				1. Cross concept conversion.
					C Series to UTF Series,
					UTF Series to C Series.

				2. Cross char-type conversion.
				3. UTF to UTF conversion: DISALLOWED!
				4. Same types: COPY.
				*/

				if ((s32)DestCode >= (s32)ECodeType::UTF8 &&
					(s32)SrcCode >= (s32)ECodeType::UTF8)
				{
					// DISALLOWED conversion.
					return -1;
				}

				if (DestCode == SrcCode)
				{
					ssize_t Size = 0;

					switch (DestCode) {
					case ECodeType::C8:		Size = TStandardStrings<c8>::Strlen((const c8*)Src); break;
					case ECodeType::C16:	Size = TStandardStrings<c16>::Strlen((const c16*)Src); break;
					case ECodeType::C32:	Size = TStandardStrings<c32>::Strlen((const c32*)Src); break;
					case ECodeType::UTF8:	Size = TStandardStrings<utf8_t>::Strlen((const utf8_t*)Src); break;
					case ECodeType::UTF16:	Size = TStandardStrings<utf16_t>::Strlen((const utf16_t*)Src); break;
					case ECodeType::UTF32:	Size = TStandardStrings<utf32_t>::Strlen((const utf32_t*)Src); break;
					default: return -1;
					}

					if (Max < 0)
						Max = Size;

					if (Size > Max)
						return Max;

					return Size;
				}

				switch (DestCode) {
				case ECodeType::C8:
				{
					switch (SrcCode)
					{
					case ECodeType::C16:
						break;
					case ECodeType::C32:
						break;
					case ECodeType::UTF8:
						break;
					case ECodeType::UTF16:
						break;
					case ECodeType::UTF32:
						break;
					}
				}
					break;
				case ECodeType::C16:
					break;
				case ECodeType::C32:
					break;
					// ----------------------
				case ECodeType::UTF8:
					break;
				case ECodeType::UTF16:
					break;
				case ECodeType::UTF32:
					break;

				default:
					break;
				}

				return -1;
			}

			ssize_t TCodecvtC2C::Convert(ECodeType DestCode, void * Dst, const void * Src, ssize_t Max, ECodeType SrcCode)
			{
				if ((s32)DestCode >= (s32)ECodeType::UTF8 &&
					(s32)SrcCode >= (s32)ECodeType::UTF8)
				{
					// DISALLOWED conversion.
					return -1;
				}

				if (DestCode == SrcCode)
				{
					ssize_t Size = 0;
					ssize_t Multiplier = 0;

					switch (DestCode) {
					case ECodeType::C8:		Size = TStandardStrings<c8>::Strlen((const c8*)Src); Multiplier = sizeof(c8); break;
					case ECodeType::C16:	Size = TStandardStrings<c16>::Strlen((const c16*)Src); Multiplier = sizeof(c16); break;
					case ECodeType::C32:	Size = TStandardStrings<c32>::Strlen((const c32*)Src); Multiplier = sizeof(c32); break;
					case ECodeType::UTF8:	Size = TStandardStrings<utf8_t>::Strlen((const utf8_t*)Src); Multiplier = sizeof(utf8_t); break;
					case ECodeType::UTF16:	Size = TStandardStrings<utf16_t>::Strlen((const utf16_t*)Src); Multiplier = sizeof(utf16_t); break;
					case ECodeType::UTF32:	Size = TStandardStrings<utf32_t>::Strlen((const utf32_t*)Src); Multiplier = sizeof(utf32_t); break;
					default: return -1;
					}

					if (Max < 0)
						Max = Size;

					// Just copy memory. 
					::memcpy(Dst, Src, 
						(Size > Max ? Max : Size) * Multiplier);

					if (Size > Max) {
						return Max;
					}

					return Size;
				}

			}
		}
	}
}