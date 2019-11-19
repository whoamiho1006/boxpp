#include "Worker.hpp"
#include <boxpp/internal/IBoxRuntime.hpp>

namespace boxpp {
	namespace async {
		FWorker::FThreadProxy::FThreadProxy(FWorker* Worker)
			: Worker(Worker)
		{
		}

		void FWorker::FThreadProxy::Run()
		{
			Worker->OnRunWork();
		}

		template<typename T>
		struct FEmptyDeleter_Proxy
		{
			FASTINLINE void operator()(T*) { }
		};

		FWorker::FWorker(bool bKeepRunning)
			: bKeepRunning(bKeepRunning), bExitLoop(false)
		{
			boxpp_rt::FBoxRuntime::Get()
				.RegisterWorker(this);
		}

		FWorker::~FWorker()
		{
			WaitExit();
			boxpp_rt::FBoxRuntime::Get()
				.UnregisterWorker(this);
		}

		void FWorker::Enqueue(const TSharedPtr<IRunnable, ESharedMode::Safe>& Runnable)
		{
			FBarriorScope Guard(Barrior);

			Queue.Enqueue(Runnable);
			ExecWorkThread();
		}

		void FWorker::ExecWorkThread()
		{
			FBarriorScope Guard(Barrior);
			
			if (!Proxy) {
				Proxy.Construct(this);
			}

			Barrior.Enter();
			if (!Thread) {
				Thread = MakeShared(new FThread());
			}

			if (!Thread->IsRunning()) {
				Thread->Run(Proxy.MakeShared());
			}
			Barrior.Leave();
		}

		void FWorker::OnRunWork()
		{
			/* Keep FThread object during completion. */
			TSharedPtr<FThread, ESharedMode::Safe> Thread = this->Thread;

			while (true)
			{
				TSharedPtr<IRunnable, ESharedMode::Safe> Current;

				Barrior.Enter();

				if (!Queue.Dequeue(Current)) {
					if (!bExitLoop || !bKeepRunning) {
						Thread = nullptr;
						Barrior.Leave();
						break;
					}
				}

				Barrior.Leave();
				
				if (!Current) {
					// Give panalty (10 ms)
					FThread::Sleep(10);
				}

				else {
					Current->Run();

					// Yield flow once.
					FThread::YieldOnce();
				}
			}
		}
	}
}
