#include "Threaded.hpp"

#include <boxpp/base/systems/Barrior.hpp>

namespace boxpp {

	struct FThreadParameters
	{
		FRawEvent* Event;
		IThreaded* Threaded;
	};

	void* FThread::GetCurrent() {
		void* RawHandle = nullptr;

#if PLATFORM_WINDOWS
		RawHandle = w32_compat::GetCurrentThread();
#endif
#if PLATFORM_POSIX
		RawHandle = pthread_self();
#endif

		return RawHandle;
	}

	void FThread::Sleep(u32 Timeout) {
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

	void FThread::YieldOnce() {
#if PLATFORM_WINDOWS
		w32_compat::Sleep(0);
#endif
#if PLATFORM_POSIX
		pthread_yield();
#endif
	}

	FRawTls& IThreaded::Tls()
	{
		static FRawTls _Tls;
		return _Tls;
	}

	IThreaded::IThreaded()
		: RawHandle(nullptr) 
	{ 
	}

	IThreaded::~IThreaded() {
		if (RawHandle) {
			Completion.Wait();
		}
	}

	bool IThreaded::Run()
	{
		static FThreadParameters Params;
		static FRawEvent Event;
		static FBarrior Barrior;
		bool bSuccess = false;

		if (!RawHandle) {
			FBarriorScope Guard(Barrior);
			Params.Threaded = this;
			(Params.Event = &Event)->Reset();

			// Reset completion state.
			Completion.Reset();

#if PLATFORM_WINDOWS
			void* hThread = w32_compat::CreateThread(nullptr,
				2048 * 10, ThreadProxy_W32, &Params, 0, nullptr);
			bSuccess = hThread != nullptr;
#endif
#if PLATFORM_POSIX
			pthread_t Thread;
			bSuccess = !pthread_create(&Thread, nullptr,
				ThreadProxy_POSIX, &Params);
#endif

			if (bSuccess) {
				Event.Wait();
			}
		}

		return bSuccess;
	}

#if PLATFORM_WINDOWS
	w32_compat::DWORD BOX_STDCALL IThreaded::ThreadProxy_W32(w32_compat::LPVOID Arg) {
		IThreaded::ThreadProxy(Arg);
		return 0;
	}
#endif
#if PLATFORM_POSIX
	void* IThreaded::ThreadProxy_POSIX(void * Arg) {
		IThreaded::ThreadProxy(Arg);
		return nullptr;
	}
#endif

	void IThreaded::ThreadProxy(void* Arg)
	{
		FThreadParameters* Params = (FThreadParameters*)Arg;
		IThreaded* Threaded = Params->Threaded;

		Threaded->RawHandle = FThread::GetCurrent();
		Tls().Set(Threaded);

		if (Params->Event) {
			Params->Event->Set();
			FThread::YieldOnce();
		}

		Threaded->OnRun();
		Threaded->Completion.Set();
		
		Tls().Set(nullptr);
	}
}