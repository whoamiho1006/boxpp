#include "NativeStrings.hpp"

#if PLATFORM_WINDOWS
#include "../boxpp-iconv/ported-headers/iconv.h"
//#pragma comment(lib, "boxpp-iconv.lib")
#endif
#if PLATFORM_POSIX && !BOX_NOT_COMPILED
#include <iconv.h>
#endif

/*

DEPRECATED for Windows.
Currently, this implementations are left only for posix kinds.

*/

#if PLATFORM_POSIX 
constexpr const char* ICONV_WCHAR_T = "WCHAR_T";
constexpr const char* ICONV_UTF8 = "UTF-8";

namespace boxpp {
	namespace _ {
		BOXPP ssize_t FStringConverter_Iconv::Measure(
			EIconvEnc DestEnc, EIconvEnc SrcEnc,
			const void* Src, ssize_t Max)
		{
			iconv_t cd = iconv_open(
				DestEnc == EIconvEnc::WideType ? ICONV_WCHAR_T : ICONV_UTF8,
				SrcEnc == EIconvEnc::WideType ? ICONV_WCHAR_T : ICONV_UTF8);

			if (cd == (iconv_t)(-1)) {
				return -1;
			}

			c8 MeasureBuffer[128];

			char* _Dest = (char*)MeasureBuffer;
			char* _Src = (char*)Src;

			size_t _SrcLeft = SrcEnc == EIconvEnc::WideType ? 
					::wcslen((wchar_t*)Src) * sizeof(wchar_t) :
					::strlen((char*)Src) * sizeof(char);

			size_t _DestLeft = 128;
			size_t _AvailBytes = 0;

			if (Max < 0)
				Max = _SrcLeft;

			if (_SrcLeft > Max)
				_SrcLeft = Max;

			while (_SrcLeft > 0) {
				size_t R = iconv(cd, 
					(const char**)&_Src, &_SrcLeft,
					(char**)&_Dest, &_DestLeft);

				if (R == (size_t)-1) {
					break;
				}

				_AvailBytes += (128 - _DestLeft);

				// Reset destination.
				_Dest = (char*)MeasureBuffer;
				_DestLeft = 128;
			}

			iconv_close(cd);

			return _AvailBytes / 
				(DestEnc == EIconvEnc::WideType ? 
				sizeof(wchar_t) : sizeof(char));
		}

		BOXPP ssize_t FStringConverter_Iconv::Convert(EIconvEnc DestEnc,
			EIconvEnc SrcEnc, void* Dest, const void* Src, ssize_t Max)
		{
			iconv_t cd = iconv_open(
				DestEnc == EIconvEnc::WideType ? ICONV_WCHAR_T : ICONV_UTF8,
				SrcEnc == EIconvEnc::WideType ? ICONV_WCHAR_T : ICONV_UTF8);

			if (cd == (iconv_t)(-1)) {
				return -1;
			}

			char* _Dest = (char*)Dest;
			char* _Src = (char*)Src;

			size_t _SrcLeft = SrcEnc == EIconvEnc::WideType ?
				::wcslen((wchar_t*)Src) * sizeof(wchar_t) :
				::strlen((char*)Src) * sizeof(char);

			size_t _DestLeft = type_db::TNumberLimits<size_t>::Max;
			size_t _AvailBytes = 0;

			if (Max < 0)
				Max = _SrcLeft;

			if (_SrcLeft > Max)
				_SrcLeft = Max;

			while (_SrcLeft > 0) {
				size_t R = iconv(cd,
					(const char**)&_Src, &_SrcLeft,
					(char**)&_Dest, &_DestLeft);

				if (R == (size_t)-1) {
					break;
				}

				_AvailBytes += (128 - _DestLeft);
			}

			iconv_close(cd);
			return _AvailBytes /
				(DestEnc == EIconvEnc::WideType ?
				sizeof(wchar_t) : sizeof(char));
		}
	}
}
#endif