#include "Debugger.hpp"

#include <boxpp/hints/windows.hpp>
#include <boxpp/hints/posix.hpp>

#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
#include <sys/types.h>
#include <sys/ptrace.h>
#else
#define ptrace(a, b, c, d)	0
#endif

bool __ptrace_test_debugger()
{
	if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0)
		return true;

	else ptrace(PTRACE_DETACH, 0, 1, 0);
	return false;
}
#endif

namespace boxpp {
	s32 FDebugger::bPresent = -1;

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
	
	void FDebugger::Break()
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
	}

	void FDebugger::Abort()
	{
		std::abort();
	}
}