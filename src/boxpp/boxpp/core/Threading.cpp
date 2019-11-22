#include "Threading.hpp"

#include <boxpp/core/RawEvent.hpp>
#include <boxpp/core/Barrior.hpp>

#include <boxpp/core/Threaded.hpp>

#include <boxpp/hints/windows.hpp>
#include <boxpp/hints/pthread.hpp>

namespace boxpp {

	struct FThreadParameters
	{
		FRawEvent* Event;
		IThreaded* Threaded;
	};

	class FThreadingOpacity
	{
	public:
#if PLATFORM_WINDOWS
		static w32_compat::DWORD BOX_STDCALL ThreadProxy_W32(w32_compat::LPVOID Arg) {
			FThreading::ThreadProxy(Arg);
			return 0;
		}
#endif
#if PLATFORM_POSIX
		static void* ThreadProxy_POSIX(void* Arg) {
			FThreading::ThreadProxy(Arg);
			return nullptr;
		}
#endif
	};
	
	void FThreading::ThreadProxy(void* Arg) {
		FThreadParameters* Params = (FThreadParameters*)Arg;
		IThreaded* Threaded = Params->Threaded;

		if (Params->Event) {
			Params->Event->Set();
			YieldOnce();
		}

		IThreaded::Deploy::Configure(Threaded);
		Threaded->OnRun();
	}

	bool FThreading::Run(IThreaded* Threaded) {
		static FThreadParameters Params;
		static FRawEvent Event;
		static FBarrior Barrior;
		bool bSuccess = false;

		FBarriorScope Guard(Barrior);
		Params.Threaded = Threaded;
		(Params.Event = &Event)->Reset();

#if PLATFORM_WINDOWS
		void* hThread = w32_compat::CreateThread(nullptr,
			2048 * 10, FThreadingOpacity::ThreadProxy_W32, &Params, 0, NULL);
		bSuccess = hThread != nullptr;
#endif
#if PLATFORM_POSIX
		pthread_t Thread;
		bSuccess = !pthread_create(&Thread, nullptr, 
			FThreadingOpacity::ThreadProxy_POSIX, &Params);
#endif

		if (bSuccess) {
			Event.Wait();
		}

		return bSuccess;
	}

	void* FThreading::GetCurrentRunning()
	{
		void* RetVal = nullptr;

#if PLATFORM_WINDOWS
		RetVal = w32_compat::GetCurrentThread();
#endif
#if PLATFORM_POSIX
		RetVal = pthread_self();
#endif

		return RetVal;
	}

	void FThreading::Sleep(u32 Timeout)
	{
#if PLATFORM_WINDOWS
		w32_compat::Sleep(Timeout);
#endif
#if PLATFORM_POSIX
		if (!Timeout)
			pthread_yield();

		else {
			usleep((Timeout % 1000) * 1000);
			sleep(Timeout / 1000);
		}
#endif
	}

	void FThreading::YieldOnce()
	{
#if PLATFORM_WINDOWS
		w32_compat::Sleep(0);
#endif
#if PLATFORM_POSIX
		pthread_yield();
#endif
	}

}