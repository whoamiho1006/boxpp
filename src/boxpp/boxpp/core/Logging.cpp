#include "Logging.hpp"

#include <boxpp/core/Debugger.hpp>
#include <boxpp/hints/vsprintf.hpp>

#include <boxpp/hints/windows.hpp>
#include <boxpp/hints/posix.hpp>

namespace boxpp
{
	FLogging& FLogging::Get()
	{
		static FLogging Logging;
		return Logging;
	}

	void FLogging::WriteFormat(ELog LogLevel, const char_t* File, s32 Line,
		const char_t* Category, const char_t* FormatString, ...)
	{
		FBarriorScope Guard(Barrior);
		va_list va;

		::memset(Buffer, 0, sizeof(Buffer));

		va_start(va, FormatString);
		TVsprintf<char_t>::Vsprintf(Buffer, FormatString, va);
		va_end(va);

		Write(LogLevel, File, Line, Category, Buffer);
	}

	void FLogging::Write(ELog LogLevel, const char_t* File, s32 Line, 
		const char_t* Category, const char_t* Message)
	{
		FBarriorScope Guard(Barrior);
		const char_t* LogLevel_Text = nullptr;

		switch (LogLevel)
		{
		case ELog::Note:
			LogLevel_Text = _::TBypass<char_t>::Select("note", L"note");
			break;

		case ELog::Warning:
			LogLevel_Text = _::TBypass<char_t>::Select("warning", L"warning");
			break;

		case ELog::Error:
			LogLevel_Text = _::TBypass<char_t>::Select("error", L"error");
			break;

		case ELog::Fatal:
			LogLevel_Text = _::TBypass<char_t>::Select("fatal", L"fatal");
			break;
		}

		for (ILogging* Logger : Loggers)
		{
			if (Logger) 
				Logger->Write(LogLevel, Category, Message);
		}

		if (IsUnderCLI()) {
			TVsprintf<char_t>::Fprintf(stderr, BOXTEXT("%s(%d): %s: %s: %s\n"),
				File, Line, LogLevel_Text, Category, Message);
		}

		if (LogLevel == ELog::Fatal)
		{
#if PLATFORM_WINDOWS
			if (!IsUnderCLI()) {
				char_t Title[1024] = { 0, };

				TVsprintf<char_t>::Sprintf(Title,
					_::TBypass<char_t>::Select("%s: %s", L"%s: %s"),
					LogLevel_Text, Category);

				w32_compat::MessageBox(nullptr, Message, Title, 0);
			}
#endif
		}

		if (LogLevel == ELog::Warning ||
			LogLevel == ELog::Fatal)
		{
			FDebugger::Break();
		}

		if (LogLevel == ELog::Fatal)
		{
			FDebugger::Abort();
		}
	}

}