#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	class IThreaded;
#if BOX_COMPILE_BODY
	class FThreadingOpacity;
#endif
	/*
		Threading sub-system.
	*/
	class BOXPP FThreading
	{
#if BOX_COMPILE_BODY
		friend class FThreadingOpacity;
#endif
	protected:
		static void ThreadProxy(void* Arg);

	public:
		static bool Run(IThreaded* Threaded);

	public:
		static void* GetCurrentRunning();
		static void Sleep(u32 Timeout = 0);
		static void YieldOnce();
	};
}