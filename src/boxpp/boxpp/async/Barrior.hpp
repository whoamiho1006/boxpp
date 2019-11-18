#ifndef __BOXPP_BARRIOR_HPP__
#define __BOXPP_BARRIOR_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

/* Loads hints/windows.hpp. */
#include <boxpp/hints/windows.hpp>

/* Loads hints/pthread.hpp. */
#include <boxpp/hints/pthread.hpp>

namespace boxpp {
	/*	Barrior is for guarding specific blocks. 
		Note: FBarror will not be compiled into shared library. */
	class BOXPP FBarrior
	{
	public:
		FASTINLINE FBarrior() {
#if PLATFORM_WINDOWS
			w32_compat::InitializeCriticalSection(&__CS__);
#endif
#if PLATFORM_POSIX
			pthread_mutex_init(&__MUTEX__, nullptr);
#endif
		}

		FASTINLINE ~FBarrior() {
#if PLATFORM_WINDOWS
			w32_compat::DeleteCriticalSection(&__CS__);
#endif
#if PLATFORM_POSIX
			pthread_mutex_destroy(&__MUTEX__);
#endif
		}

	public:
		/* Enter to behind of barrior. */
		FASTINLINE void Enter() const {
#if PLATFORM_WINDOWS
			if (!w32_compat::TryEnterCriticalSection(&__CS__))
				w32_compat::EnterCriticalSection(&__CS__);
#endif
#if PLATFORM_POSIX
			if (pthread_mutex_trylock(&__MUTEX__))
				pthread_mutex_lock(&__MUTEX__);
#endif
		}

		/* Try enter to behind of barrior. */
		FASTINLINE bool TryEnter() const {
			register bool RetVal = false;

#if PLATFORM_WINDOWS
			RetVal = w32_compat::TryEnterCriticalSection(&__CS__);
#endif
#if PLATFORM_POSIX
			RetVal = !pthread_mutex_trylock(&__MUTEX__);
#endif
			return RetVal;
		}

		/* Leave from behind of barrior. */
		FASTINLINE void Leave() const {
#if PLATFORM_WINDOWS
			w32_compat::LeaveCriticalSection(&__CS__);
#endif
#if PLATFORM_POSIX
			pthread_mutex_unlock(&__MUTEX__);
#endif
		}

	private:
#if PLATFORM_WINDOWS
		mutable w32_compat::CRITICAL_SECTION __CS__;
#endif
#if PLATFORM_POSIX
		mutable pthread_mutex_t __MUTEX__;
#endif
	};

	/* Barrior scope. */
	class FBarriorScope
	{
	public:
		FASTINLINE FBarriorScope(FBarrior& Barrior) 
			: Barrior(&Barrior)
		{
			Barrior.Enter();
		}

		FASTINLINE FBarriorScope(const FBarrior& Barrior)
			: Barrior(&Barrior)
		{
			Barrior.Enter();
		}

		FASTINLINE ~FBarriorScope() 
		{
			Barrior->Leave();
		}

	private:
		const FBarrior* Barrior;
	};

#define BOX_BARRIOR_SCOPED(BarriorInstance) \
	boxpp::FBarriorScope BOX_CONCAT(__Scope_At_, __LINE__) (BarriorInstance)

#define BOX_DO_WITH_BARRIOR(BarriorInstance, ...) \
	if (true) { BOX_BARRIOR_SCOPED(BarriorInstance); __VA_ARGS__ }
}
#endif