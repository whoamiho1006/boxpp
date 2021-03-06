#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>

namespace boxpp {
	/*	Barrior is for guarding specific blocks. */
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