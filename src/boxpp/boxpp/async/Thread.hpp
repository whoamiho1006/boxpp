#ifndef __BOXPP_ASYNC_THREAD_HPP__
#define __BOXPP_ASYNC_THREAD_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

/* Loads hints/windows.hpp. */
#include <boxpp/hints/windows.hpp>

/* Loads hints/pthread.hpp. */
#include <boxpp/hints/pthread.hpp>

#ifndef __BOXPP_SHAREDPTR_BORADCASTED_HPP__
#include <boxpp/sharedptr/BroadcastedObject.hpp>
#endif

#include <boxpp/async/IRunnable.hpp>
#include <boxpp/utils/Instrusive.hpp>

#include <boxpp/async/Barrior.hpp>
#include <boxpp/async/Event.hpp>

namespace boxpp {
	namespace async {

		/* Thread. */
		class FThread : public IBroadcastedObject<FThread, ESharedMode::Safe>
		{
		private:
#if PLATFORM_WINDOWS
			static w32_compat::DWORD BOX_STDCALL ThreadProxy_W32(w32_compat::LPVOID Arg) {
				((FThread*)Arg)->OnRun();
				return 0;
			}
#endif
#if PLATFORM_POSIX
			static void* ThreadProxy_POSIX(void* Arg) {
				((FThread*)Arg)->OnRun();
				return nullptr;
			}
#endif

		public:
			FASTINLINE FThread()
				: Runnable(nullptr), bRunning(false)
			{
			}

			FASTINLINE FThread(const TSharedPtr<IRunnable>& Runnable, bool bImmediate = true)
				: Runnable(Runnable), bRunning(false)
			{
				if (bImmediate && Runnable) {
					Run();
				}
			}

			FASTINLINE ~FThread()
			{
				Wait();
			}

		private:
			FThread(const FThread&) = delete;
			FThread(FThread&&) = delete;

		public:
			/* Determines this thread is running or not. */
			FASTINLINE bool IsRunning() const { return bRunning && Runnable; }

		public:
			/* Run thread. */
			FASTINLINE bool Run() {
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

			/* Run thread. */
			FASTINLINE bool Run(const TSharedPtr<IRunnable>& Runnable) {
				if (!bRunning) {
					this->Runnable = Runnable;
					return Run();
				}

				return false;
			}

			/* Wait thread completion. */
			FASTINLINE bool Wait() {
				bool bRetVal = false;

#if PLATFORM_WINDOWS
				if (hThread && bRunning) {
					w32_compat::WaitForSingleObject(hThread, w32_compat::COMPAT_Infinite);
					bRetVal = true;
				}
#endif
#if PLATFORM_POSIX
				if (bRunning) {
					pthread_join(Thread, nullptr);
					bRetVal = true;
				}
#endif
				if (Event) {
					Event.Destruct();
				}

				return bRetVal;
			}

		private:
			FASTINLINE void OnRun()
			{
				if (Event) {
					Event->Set();
				}

				if (Runnable) {
					Runnable->Run();
				}

				Barrior.Enter();
				bRunning = false;
				Barrior.Leave();
			}

		public:
			FASTINLINE FBarrior& GetBarrior() { return Barrior; }
			FASTINLINE const FBarrior& GetBarrior() const { return Barrior; }

		private:
			TSharedPtr<IRunnable, ESharedMode::Safe> Runnable;
			bool bRunning;

#if PLATFORM_WINDOWS
			w32_compat::HANDLE hThread;
#endif
#if PLATFORM_POSIX
			pthread_t Thread;
#endif

			TInstrusive<FEvent> Event;
			FBarrior Barrior;
		};

	}
}

#endif // !__BOXPP_ASYNC_THREAD_HPP__