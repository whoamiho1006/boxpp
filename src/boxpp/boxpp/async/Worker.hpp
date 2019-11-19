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
#include <boxpp/async/ThreadLocal.hpp>

#if BOX_COMPILE_BODY
namespace boxpp_rt {
	class FBoxRuntime;
}
#endif

namespace boxpp {
	namespace async {
		class IRunnable;

		class IWorker
		{
		public:
			virtual ~IWorker() { }

		public:
			virtual void Enqueue(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable) = 0;
		};

		class BOXPP FWorker : public IWorker
		{
#if BOX_COMPILE_BODY
		protected:
			friend class boxpp_rt::FBoxRuntime;
#endif

		public:
			FWorker(bool bKeepRunning = false);
			~FWorker();

		private:
			class FThreadProxy : public IRunnable {
			public:
				FThreadProxy(FWorker* Worker);

			public:
				virtual void Run();

			private:
				FWorker* Worker;
			};

		public:
			virtual void Enqueue(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable) override;

		private:
			void ExecWorkThread();
			void OnRunWork();

		private:
			FBarrior Barrior;
			TSharedPtr<FThread, ESharedMode::Safe> Thread;
			bool bKeepRunning, bExitLoop;

			TInstrusive<FThreadProxy> Proxy;
			TQueue<TSharedPtr<IRunnable, ESharedMode::Safe>> Queue;
			
		protected:
			FASTINLINE void WaitExit() {
				TSharedPtr<FThread, ESharedMode::Safe> Thread = this->Thread;
				FBarriorScope Guard(Barrior);
				bExitLoop = true;

				if (Thread && Thread->IsRunning()) {
					Thread->Wait();
				}
			}
		};

	}
}

#endif // !__BOXPP_ASYNC_ASYNC_HPP__