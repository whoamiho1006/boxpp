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
		class BOXPP FThread : public IBroadcastedObject<FThread, ESharedMode::Safe>
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
			/* Get native thread handle of current running thread. */
			static void* Self() {
				void* Current = nullptr;

#if PLATFORM_WINDOWS
				Current = w32_compat::GetCurrentThread();
#endif
#if PLATFORM_POSIX
				Current = pthread_self();
#endif

				return Current;
			}

		public:
			FASTINLINE FThread()
				: Runnable(nullptr), bRunning(false)
			{
			}

			FASTINLINE FThread(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable, bool bImmediate = true)
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
			FASTINLINE void* GetRaw() const { return bRunning ? NativeSelf : nullptr; }

		public:
			FASTINLINE FBarrior& GetBarrior() { return Barrior; }
			FASTINLINE const FBarrior& GetBarrior() const { return Barrior; }

		public:
			FASTINLINE static void Sleep(u32 Timeout = 0) {
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

			FASTINLINE static void YieldOnce() {
#if PLATFORM_WINDOWS
				w32_compat::Sleep(0);
#endif

#if PLATFORM_POSIX
				pthread_yield();
#endif
			}

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
			FASTINLINE bool Run(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable) {
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
				NativeSelf = Self();

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
			}

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

			void* NativeSelf;

		private:
			void OnThreadShutdown();
		};

		class FSharedThread
		{
		public:
			FSharedThread(nullptr_t = nullptr) { }

			FASTINLINE FSharedThread(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable, bool bImmediate = true)
				: Thread(MakeShared(new FThread(Runnable, bImmediate)))
			{
			}

			FASTINLINE FSharedThread(const FSharedThread& Other)
				: Thread(Other.Thread)
			{
			}

			FASTINLINE FSharedThread(FSharedThread&& Other)
				: Thread(Forward<TSharedPtr<FThread, ESharedMode::Safe>>(Other.Thread))
			{
			}

			FASTINLINE ~FSharedThread()
			{
			}

		public:
			FASTINLINE operator bool() const { return Thread && Thread->IsRunning(); }
			FASTINLINE bool operator !() const { return !Thread || !Thread->IsRunning(); }

			/* Determines this thread is running or not. */
			FASTINLINE bool IsRunning() const { return Thread && Thread->IsRunning(); }
			FASTINLINE void* GetRaw() const { return Thread ? Thread->GetRaw() : nullptr; }

			FASTINLINE bool operator ==(const FSharedThread& Other) const { return Thread == Other.Thread; }
			FASTINLINE bool operator !=(const FSharedThread& Other) const { return Thread != Other.Thread; }

		public:
			FASTINLINE FSharedThread& operator =(nullptr_t) { Thread = nullptr; return *this; }
			FASTINLINE FSharedThread& operator =(const FSharedThread& Other) { Thread = Other.Thread; return *this; }
			FASTINLINE FSharedThread& operator =(FSharedThread&& Other) {
				Thread = Forward<TSharedPtr<FThread, ESharedMode::Safe>>(Other.Thread);
				return *this;
			}

		public:
			/* Run thread. */
			FASTINLINE bool Run() {
				return Thread && Thread->Run();
			}

			/* Run thread. */
			FASTINLINE bool Run(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable) {
				if (!Thread) {
					Thread = MakeShared(new FThread(Runnable, false));
				}

				return Run();
			}

			/* Wait thread completion. */
			FASTINLINE bool Wait() {
				return Thread && Thread->Wait();
			}

		private:
			TSharedPtr<FThread, ESharedMode::Safe> Thread;
		};

	}
}

#endif // !__BOXPP_ASYNC_THREAD_HPP__