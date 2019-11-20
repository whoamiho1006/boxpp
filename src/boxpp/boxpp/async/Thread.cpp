#include "Thread.hpp"
#include "ThreadLocal.hpp"
#include <boxpp/internal/IBoxRuntime.hpp>

namespace boxpp {
	namespace async {
#if PLATFORM_WINDOWS
		w32_compat::DWORD BOX_STDCALL FThread::ThreadProxy_W32(w32_compat::LPVOID Arg) {
			((FThread*)Arg)->OnRun();
			return 0;
		}
#endif
#if PLATFORM_POSIX
		void* FThread::ThreadProxy_POSIX(void* Arg) {
			((FThread*)Arg)->OnRun();
			return nullptr;
		}
#endif

		TThreadLocal<FThread>& FThread::GetTLS() {
			static TThreadLocal<FThread> TLS;
			return TLS;
		}

		void* FThread::SelfNative() {
			void* Current = nullptr;

#if PLATFORM_WINDOWS
			Current = w32_compat::GetCurrentThread();
#endif
#if PLATFORM_POSIX
			Current = pthread_self();
#endif

			return Current;
		}

		bool FThread::IsUnderExeThread() {
			return SelfNative() == boxpp_rt::FBoxRuntime::GetExeThread();
		}

		FThread* FThread::Self() {
			return GetTLS().Get();
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

		bool FThread::Run() {
			if (!bRunning && Runnable) {
				if (!Event) {
					Event.Construct();
				}

				Event->Reset();

#if PLATFORM_WINDOWS
				Barrior.Enter();
				hThread = w32_compat::CreateThread(nullptr,
					2048 * 10, ThreadProxy_W32, this, 0, NULL);

				bRunning = hThread != nullptr;
				Barrior.Leave();
#endif
#if PLATFORM_POSIX
				bRunning = !pthread_create(&Thread, nullptr, ThreadProxy_POSIX, this);
#endif
				Event->Wait();
				return true;
			}
			return false;
		}

		void FThread::OnRun()
		{
			NativeSelf = SelfNative();
			GetTLS().Set(this);

			if (Event) {
				Event->Set();

				/*
				Yield once for giving initialization
				chances of extra implementations.
				*/
				YieldOnce();
			}

			if (Runnable) {
				Runnable->Run();
			}
			
			Barrior.Enter();
			bRunning = false;
			Barrior.Leave();

			OnThreadShutdown();
			GetTLS().Set(nullptr);
		}

		void FThread::OnThreadShutdown()
		{
			_::FThreadLocalServer::Get().Shutdown(this);
		}
	}
}
