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

		public:
			FASTINLINE size_t GetMaxWorkers() const { return MaxWorkers; }
			FASTINLINE size_t GetMinWorkers() const { return MinWorkers; }
			FASTINLINE bool IsAutoScalable() const { return bAllowAuto; }

			FASTINLINE void SetAutoScalable(bool bAllow, size_t Max, size_t Min)
			{
				FBarriorScope Guard(Barrior);
				MaxWorkers = Max; MinWorkers = Min;
				bAllowAuto = bAllow;
			}

			FASTINLINE void SetCutline(float Max = 1.0f, float Min = -1.0f)
			{
				FBarriorScope Guard(Barrior);
				LoadMax = Max;
				LoadMin = Min;
			}

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
			FBarrior Barrior;
			FName Name;

			bool bAllowAuto;
			size_t MaxWorkers, MinWorkers;
			float DelayRate, DelayMax;

			/* Dead-line to determine workers increase/decrease worker or not.*/
			float LoadMax, LoadMin;

			TSortedArray<FWorker*> Workers;
			TSortedArray<FWorker*> ScaledWorkers;

			TQueue<TSharedPtr<IRunnable, ESharedMode::Safe>> Queue;

		};
	}
}

#endif // !__BOXPP_ASYNC_WORKERGROUP_HPP__