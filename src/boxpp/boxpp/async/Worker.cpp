#include "Worker.hpp"

namespace boxpp {
	namespace async {
		FWorker::FThreadProxy::FThreadProxy(FWorker& Worker)
			: Worker(Worker)
		{
		}

		void FWorker::FThreadProxy::Run()
		{
			Worker.OnRunWork();
		}

		template<typename T>
		struct FEmptyDeleter_Proxy
		{
			FASTINLINE void operator()(T*) { }
		};

		FWorker::~FWorker()
		{
			if (Thread.IsRunning()) {
				Thread.Wait();
			}
		}

		void FWorker::ExecWorkThread()
		{
			FBarriorScope Guard(Barrior);
			FEmptyDeleter_Proxy<FThreadProxy> EmptyDeleter;

			if (!Proxy) {
				Proxy.Construct();
			}

			if (!Thread.IsRunning()) {
				Thread.Run(Proxy.MakeShared());
			}
		}

		void FWorker::OnRunWork()
		{
			while (true)
			{
				TSharedPtr<IRunnable, ESharedMode::Safe> Current;

				Barrior.Enter();

				if (!Queue.Dequeue(Current)) {
					if (!bKeepRunning) {
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
