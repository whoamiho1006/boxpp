#ifndef __BOXPP_ASYNC_WORKER_HPP__
#define __BOXPP_ASYNC_WORKER_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Barrior.hpp>
#include <boxpp/containers/Queue.hpp>

#include <boxpp/async/Task.hpp>
#include <boxpp/async/TaskSource.hpp>
#include <boxpp/async/Thread.hpp>

namespace boxpp {
	namespace async {
		class IRunnable;

		class BOXPP FWorker
		{
		public:
			FWorker(bool bKeepRunning = false)
				: bKeepRunning(bKeepRunning) { }

			~FWorker();

		private:
			class FThreadProxy : public IRunnable {
			public:
				FThreadProxy(FWorker& Worker);

			public:
				virtual void Run();

			private:
				FWorker& Worker;
			};

			struct FWorkerLocal {
				void* Value;
				void(*Cleanup)(void*);
			};

		public:
			FASTINLINE void Enqueue(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable) {
				FBarriorScope Guard(Barrior);

				Queue.Enqueue(Runnable);
				ExecWorkThread();
			}

		private:
			void ExecWorkThread();
			void OnRunWork();

		private:
			FBarrior Barrior;
			FThread Thread;
			bool bKeepRunning;

			TArray<FWorkerLocal> WorkerLocals;

			TInstrusive<FThreadProxy> Proxy;
			TQueue<TSharedPtr<IRunnable, ESharedMode::Safe>> Queue;
		};

	}
}

#endif // !__BOXPP_ASYNC_ASYNC_HPP__