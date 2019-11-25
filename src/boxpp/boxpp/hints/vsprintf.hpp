#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>

namespace boxpp
{
	template<typename CharType>
	struct TVsprintf
	{
		FASTINLINE static void Vsprintf(CharType* Buffer, const CharType* Format, ...) { }
		FASTINLINE static void Printf(CharType* Buffer, const CharType* Format, ...) { }
	};

	template<> struct TVsprintf<ansi_t> {
		FASTINLINE static void Vsprintf(ansi_t* Buffer, const ansi_t* Format, va_list va) {
			::vsprintf(Buffer, Format, va);
		}

		static void Sprintf(ansi_t* Buffer, const ansi_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			::vsprintf(Buffer, Format, va);
			va_end(va);
		}

		static void Printf(const ansi_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			vprintf(Format, va);
			va_end(va);
		}

		static void Fprintf(FILE* fp, const ansi_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			vfprintf(fp, Format, va);
			va_end(va);
		}
	};

	template<> struct TVsprintf<wide_t> {
		FASTINLINE static void Vsprintf(wide_t* Buffer, const wide_t* Format, va_list va) {
			::vswprintf(Buffer, Format, va);
		}

		static void Sprintf(wide_t* Buffer, const wide_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			::vswprintf(Buffer, Format, va);
			va_end(va);
		}

		static void Printf(const wide_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			vwprintf(Format, va);
			va_end(va);
		}

		static void Fprintf(FILE* fp, const wide_t* Format, ...) {
			va_list va;

			va_start(va, Format);
			vfwprintf(fp, Format, va);
			va_end(va);
		}
	};
}