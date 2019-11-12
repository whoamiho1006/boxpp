#ifndef __BOXPP_ASYNC_EVENT_HPP__
#define __BOXPP_ASYNC_EVENT_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

/* Loads hints/windows.hpp. */
#include <boxpp/hints/windows.hpp>

/* Loads hints/pthread.hpp. */
#include <boxpp/hints/pthread.hpp>

namespace boxpp {
	/*	Event is for guarding specific blocks.
		Note: FEvent will not be compiled into shared library. */
	class FEvent
	{
	public:
		FASTINLINE FEvent(bool bManual = true, bool bInitState = false) {
#if PLATFORM_WINDOWS
			__EVENT__ = w32_compat::CreateEventA(0, bManual, bInitState, 0);
#endif
#if PLATFORM_POSIX
			pthread_mutex_init(&__MUTEX__, nullptr);
			pthread_cond_init(&__COND__, nullptr);

			__STATE__ = bInitState;
			__MANUAL__ = bManual;
#endif
		}

		FASTINLINE ~FEvent() {
#if PLATFORM_WINDOWS
			w32_compat::CloseHandle(__EVENT__);
#endif
#if PLATFORM_POSIX
			pthread_cond_destroy(&__COND__);
			pthread_mutex_destroy(&__MUTEX__);
#endif
		}

	public:
		FASTINLINE void Wait() const {
#if PLATFORM_WINDOWS
			w32_compat::WaitForSingleObject(__EVENT__, w32_compat::COMPAT_Infinite);
#endif
		}

		FASTINLINE bool Wait(s32 Miliseconds) const {
			if (Miliseconds < 0)
			{
				Wait();
				return true;
			}

#if PLATFORM_WINDOWS
			if (w32_compat::WaitForSingleObject(__EVENT__, Miliseconds) == 0) {
				return true;
			}
#endif
#if PLATFORM_POSIX
			timespec ts;
			bool retVal = false;

			while (Miliseconds >= 0) 
			{
				if (Miliseconds >= 1000) {
					ts.tv_sec = 1;
					ts.tv_nsec = 1;
				}

				else {
					ts.tv_sec = 0;
					ts.tv_nsec = (Miliseconds % 1000) * 1000000;
				}

				{
					pthread_mutex_lock(&__MUTEX__);
					pthread_cond_timedwait(&__COND__, &__MUTEX__, &ts);

					retVal = __STATE__;
					if (!__MANUAL__ && __STATE__) {
						__STATE__ = false;
					}

					pthread_mutex_unlock(&__MUTEX__);
				}

				Miliseconds -= 1000;
				if (retVal) {
					break;
				}
			}

			if (retVal)
				return true;
#endif
			return false;
		}

		FASTINLINE void Set() {
#if PLATFORM_WINDOWS
			w32_compat::SetEvent(__EVENT__);
#endif
#if PLATFORM_POSIX
			pthread_mutex_lock(&__MUTEX__);
			__STATE__ = true;

			pthread_cond_signal(&__COND__);
			pthread_mutex_unlock(&__MUTEX__);
#endif
		}

		FASTINLINE void Reset() {
#if PLATFORM_WINDOWS
			w32_compat::ResetEvent(__EVENT__);
#endif
#if PLATFORM_POSIX
			pthread_mutex_lock(&__MUTEX__);
			__STATE__ = false;

			pthread_cond_signal(&__COND__);
			pthread_mutex_unlock(&__MUTEX__);
#endif
		}

	private:
#if PLATFORM_WINDOWS
		w32_compat::HANDLE __EVENT__;
#endif
#if PLATFORM_POSIX
		mutable pthread_mutex_t __MUTEX__;
		mutable pthread_cond_t __COND__;
		mutable bool __STATE__;
		bool __MANUAL__;
#endif
	};
}

#endif // !__BOXPP_ASYNC_EVENT_HPP__