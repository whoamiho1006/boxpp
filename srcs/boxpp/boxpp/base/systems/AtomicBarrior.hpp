#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <atomic>

namespace boxpp
{

	class FAtomicBarrior
	{
	public:
		FAtomicBarrior() { }
		~FAtomicBarrior() { }

	private:
		mutable std::atomic_flag Atomic;

	public:
		FASTINLINE void Enter() const {
			while (!Atomic.test_and_set(std::memory_order_acquire));
		}

		FASTINLINE bool TryEnter() const {
			return Atomic.test_and_set(std::memory_order_acquire);
		}

		FASTINLINE void Leave() const {
			Atomic.clear(std::memory_order_release);
		}
	};

	/* Barrior scope. */
	class FAtomicScope
	{
	public:
		FASTINLINE FAtomicScope(FAtomicBarrior& Barrior)
			: Barrior(&Barrior)
		{
			Barrior.Enter();
		}

		FASTINLINE FAtomicScope(const FAtomicBarrior& Barrior)
			: Barrior(&Barrior)
		{
			Barrior.Enter();
		}

		FASTINLINE ~FAtomicScope()
		{
			Barrior->Leave();
		}

	private:
		const FAtomicBarrior* Barrior;
	};
}