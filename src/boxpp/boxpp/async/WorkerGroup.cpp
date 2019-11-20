#include "WorkerGroup.hpp"
#include <boxpp/utils/PureTimer.hpp>

namespace boxpp {
	namespace async {
		FWorkerGroup::FWorkerGroup(const FName& Name)
			: Name(Name), bAllowAuto(false), MaxWorkers(10), MinWorkers(0), 
			DelayRate(0.0f), DelayMax(0.0f), LoadMax(0.0f), LoadMin(0.0f)
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
			if (ScaledWorkers.Contains(Worker)) {
				ScaledWorkers.Remove(Worker);
			}

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
		{;
			FWorker* Worker = nullptr;
			float DelayAverage = 0.0f;
			float PreviousDelay = DelayRate;
			bool bKillOne = false;

			for (FWorker* Each : Workers)
				DelayAverage += Each->GetDelayRate();

			if (Workers)
				DelayAverage /= Workers.GetSize();

			/* Calculate current delay rate. */
			DelayRate = DelayAverage * 0.75f + DelayRate * 0.25f;
			if (DelayMax < DelayRate) DelayMax = DelayRate;

			/* Delta Rate. */
			if (PreviousDelay >= 0.0f) {
				float LoadScale = (DelayRate - PreviousDelay) / PreviousDelay;

				/*
					If scale of loads growed up over 100% than previous load,
					create new worker.
				*/

				if (LoadScale >= 1.0f && GetWorkerSize() < MaxWorkers) {
					Worker = new FWorker(true);
				}

				/*
					If scale of loads decreased over -100% than previous load,
					clean one worker up.
				*/
				bKillOne = LoadScale <= -1.0f && GetWorkerSize() > MinWorkers;
			}

			if (Worker) {
				Workers.Add(Worker);
				ScaledWorkers.AddUnique(Worker);
				return true;
			}

			if (bKillOne && ScaledWorkers.GetSize()) {
				Worker = ScaledWorkers[0];

				Unregister(Worker);
				delete Worker;
			}
			
			return false;
		}
	}
}
