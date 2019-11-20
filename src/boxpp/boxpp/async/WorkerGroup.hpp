#ifndef __BOXPP_ASYNC_WORKERGROUP_HPP__
#define __BOXPP_ASYNC_WORKERGROUP_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Worker.hpp>

namespace boxpp
{
	namespace async
	{
		class BOXPP FWorkerGroup
		{
			friend class FWorker;

		public:
			FWorkerGroup(const FName& Name = FName::Unnamed);
			~FWorkerGroup();

		public:
			void Register(FWorker* Worker);
			void Unregister(FWorker* Worker);

		public:
			FASTINLINE bool HasWorker() const { return Workers.GetSize() > 0; }
			FASTINLINE size_t GetWorkerSize() const { return Workers.GetSize(); }
			FASTINLINE size_t GetQueuedWorks() const { return Queue.GetSize(); }
			FASTINLINE const FName& GetName() const { return Name; }

		private:
			void OnSchedule(FWorker* Worker);

		public:
			FASTINLINE bool Enqueue(const TSharedPtr<IRunnable, ESharedMode::Safe>& Work)
			{
				FBarriorScope Guard(Barrior);

				/* If auto scaling disabled, but no workers, it fails to enqueue.*/
				if (!bAllowAuto && Workers.GetSize() <= 0)
					return false;

				/* If auto workers are not on maximum, increase worker. */
				if (Workers.GetSize() < MaxWorkers)
					ScaleWorker();

				/* after scaling routine, no workers are registered, it fails. */
				if (Workers.GetSize() <= 0)
					return false;

				Queue.Enqueue(Work);
				return true;
			}

		private:
			bool ScaleWorker();

		private:
			struct WorkerState
			{
				bool AutoCreated;
				float DelayRates;
			};

		private:
			FBarrior Barrior;
			FName Name;

			bool bAllowAuto;
			size_t MaxWorkers, MinWorkers;

			TSortedArray<FWorker*> Workers;
			TMap<FWorker*, WorkerState> WorkerStates;

			TQueue<TSharedPtr<IRunnable, ESharedMode::Safe>> Queue;

		};
	}
}

#endif // !__BOXPP_ASYNC_WORKERGROUP_HPP__