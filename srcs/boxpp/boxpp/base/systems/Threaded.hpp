#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>

#include <boxpp/base/systems/RawEvent.hpp>
#include <boxpp/base/systems/RawTls.hpp>

namespace boxpp {

	/*
		Thread utilities.
	*/
	class BOXPP FThread
	{
	public:
		/* Get current thread's raw handle. */
		static void* GetCurrent();

		/* Sleep current running thread. */
		static void Sleep(u32 Timeout);

		/* Yield flow once to other thread. */
		static void YieldOnce();
	};

	/*
		IThreaded interface.
		This interface provides common interface that can be THREADED.
	*/
	class BOXPP IThreaded
	{
	private:
		static FRawTls& Tls();

	public:
		IThreaded();
		virtual ~IThreaded();

	public:
		/* Get current threaded object.*/
		FASTINLINE static IThreaded* GetCurrent() {
			return (IThreaded*)Tls().Init();
		}

	public:
		/* Run this threaded object. */
		bool Run();

		/* Determines this threaded object was ran or not. */
		FASTINLINE bool HasRan() const {
			return RawHandle;
		}

		/* Determines this threaded object is alive or not. */
		FASTINLINE bool IsAlive() const {
			return RawHandle && !Completion.Wait(0);
		}

		/* Wait completion during miliseconds. */
		FASTINLINE bool Wait(u32 Miliseconds) const {
			return RawHandle ? Completion.Wait(Miliseconds) : false;
		}

		/* Wait during completion. */
		FASTINLINE bool Wait() const {
			if (RawHandle) {
				Completion.Wait();
				return true;
			}

			return false;
		}

	protected:
		/* Entry point of threaded object. */
		virtual void OnRun() = 0;

	public:
		FASTINLINE void* GetRaw() const { return RawHandle; }
		FASTINLINE FRawEvent* GetRawEvent() const { return &Completion; }

	private:
		void* RawHandle;
		mutable FRawEvent Completion;

#if BOX_COMPILE_BODY
		/* NOTE: Hiding static methods are safe for clean-interface. */
	private:
#	if PLATFORM_WINDOWS
		static w32_compat::DWORD BOX_STDCALL ThreadProxy_W32(w32_compat::LPVOID Arg);
#	endif
#	if PLATFORM_POSIX
		static void* ThreadProxy_POSIX(void* Arg);
#	endif
		static void ThreadProxy(void* Arg);
#endif
	};
}