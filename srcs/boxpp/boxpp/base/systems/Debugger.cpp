#include "Debugger.hpp"

#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/opacities/posix.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <exception>

#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
#include <sys/types.h>
#include <sys/ptrace.h>
#else
#define ptrace(a, b, c, d)	0
#define isatty(x)	true
#endif

bool __ptrace_test_debugger()
{
	if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0)
		return true;

	else ptrace(PTRACE_DETACH, 0, 1, 0);
	return false;
}
#endif

extern void MinidumpInstall();

namespace boxpp {
	s32 FDebugger::bPresent = -1;
	s32 FDebugger::bUnderCLI = -1;

	void FDebugger::Init()
	{
		static bool bInit = false;

		if (!bInit)
		{
			bInit = true;

#if PLATFORM_WINDOWS
			MinidumpInstall();
#endif
		}
	}

	bool FDebugger::IsPresent() {
		bool RetVal = false;
		
#if PLATFORM_WINDOWS
		RetVal = w32_compat::IsDebuggerPresent();
#endif
#if PLATFORM_POSIX
		if (bPresent < 0) {
			bPresent = __ptrace_test_debugger() ? 0 : 1;
		}

		RetVal = !bPresent;
#endif

		return RetVal;
	}

	bool FDebugger::IsUnderCLI()
	{
		if (bUnderCLI < 0) {
#if PLATFORM_WINDOWS
			w32_compat::HANDLE hStdIn = w32_compat::GetStdHandle(w32_compat::COMPAT_STD_INPUT_HANDLE);
			w32_compat::HANDLE hStdOut = w32_compat::GetStdHandle(w32_compat::COMPAT_STD_OUTPUT_HANDLE);

			w32_compat::DWORD Mode;
			w32_compat::CONSOLE_SCREEN_BUFFER_INFO SBI;

			if (!w32_compat::GetConsoleMode(hStdIn, &Mode) ||
				!w32_compat::GetConsoleScreenBufferInfo(hStdOut, &SBI))

				bUnderCLI = 0;
			else
				bUnderCLI = 1;
#endif
#if PLATFORM_POSIX
			bUnderCLI = 0;

			if (isatty(fileno(stdin)) &&
				isatty(fileno(stdout)))
			{
				bUnderCLI = 1;
			}
#endif
		}

		return bUnderCLI > 0;
	}

	bool FDebugger::Break()
	{
#if PLATFORM_WINDOWS
		if (w32_compat::IsDebuggerPresent())
			w32_compat::DebugBreak();
#endif
#if PLATFORM_POSIX
		if (IsPresent()) {
#if !defined(SIGTRAP)
			__builtin_trap();
#else
			raise(SIGTRAP);
#endif
		}
#endif

		return false;
	}

	void FDebugger::Abort() {
		exit(EXIT_FAILURE);
	}

	bool FDebugger::Failure(const ansi_t* File, s32 Line,
		const ansi_t* Expression, const ansi_t* Message)
	{
		if (IsUnderCLI() || PLATFORM_POSIX) {
			fprintf(stderr, "fatal error (%s): %s: %s (%d)\n",
				Expression, Message, File, Line);
		}

#if PLATFORM_WINDOWS
		else {
			ansi_t Buffer[2048] = { 0, };

			sprintf(Buffer, 
				"%s\n" 
				"Expression: (%s) == false.\n" 
				"File: %s (%d)",
				Message, Expression, File, Line);

			w32_compat::MessageBox(nullptr, 
				Buffer, "box++: fatal error", 0x00000010L);
		}
#endif

		Break();
		throw 0;
		//Abort();

		return false;
	}
}