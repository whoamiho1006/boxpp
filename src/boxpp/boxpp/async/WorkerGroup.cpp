#include "WorkerGroup.hpp"

namespace boxpp {
	namespace async {
		FWorkerGroup::FWorkerGroup(const FName& Name)
			: Name(Name), bAllowAuto(false),
			MaxWorkers(0), MinWorkers(0)
		{
		}

		FWorkerGroup::~FWorkerGroup()
		{
			FBarriorScope Guard(Barrior);

			TArray<FWorker*> Workers = this->Workers;
			this->Workers.Clear();

			for (FWorker* Worker : Workers) {
				Worker->SetRequire(nullptr);
				Worker->SetGroup(nullptr);
			}
		}

		void FWorkerGroup::Register(FWorker* Worker)
		{
			FBarriorScope Guard(Barrior);
			Workers.Add(Worker);

			Worker->SetRequire(TFunction<void(FWorker*)>(this, &FWorkerGroup::OnSchedule));
			Worker->SetGroup(this);
		}

		void FWorkerGroup::Unregister(FWorker* Worker)
		{
			FBarriorScope Guard(Barrior);
			Workers.Remove(Worker);

			Worker->SetRequire(nullptr);
			Worker->SetGroup(nullptr);
		}

		void FWorkerGroup::OnSchedule(FWorker* Worker)
		{
			TSharedPtr<IRunnable, ESharedMode::Safe> Work;
			FBarriorScope Guard(Barrior);

			if (Queue.Dequeue(Work))
			{
				if (Work)
				{
					Worker->Enqueue(Work);
				}
			}
		}

		bool FWorkerGroup::ScaleWorker()
		{
			return false;
		}
	}
}
