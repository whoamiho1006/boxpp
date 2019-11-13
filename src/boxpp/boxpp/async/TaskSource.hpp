#ifndef __BOXPP_ASYNC_TASKSOURCE_HPP__
#define __BOXPP_ASYNC_TASKSOURCE_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Event.hpp>
#include <boxpp/async/Task.hpp>
#include <boxpp/utils/PureTimer.hpp>

namespace boxpp {
	namespace async {

		/* Task source. */
		template<typename ReturnType>
		class TTaskSource
		{
			template<typename>
			friend class TTask;

		public:
			TTaskSource() 
			{
			}

			~TTaskSource()
			{
			}

		protected:
			ETaskState TaskState;
			TInstrusive<ReturnType> ReturnValue;
			FEvent StateNotifier;

		public:
			/* Set this task completed. */
			FASTINLINE bool SetSuccess(const ReturnType& Value) {
				if (TaskState == ETaskState::None) {
					if (ReturnValue) {
						ReturnValue.Destruct();
					}

					ReturnValue.ConstructFrom(Value);
					TaskState = ETaskState::Success;
					StateNotifier.Set();
					return true;
				}

				return false;
			}

			/* Set this task failed. */
			FASTINLINE void SetFailure() {
				if (TaskState == ETaskState::None) {
					if (ReturnValue) {
						ReturnValue.Destruct();
					}

					TaskState = ETaskState::Failure;
					StateNotifier.Set();
					return true;
				}

				return false;
			}
		};
	}

	template<typename ReturnType>
	FASTINLINE bool TTask<ReturnType>::IsCompleted() const {
		return TaskSource && TaskSource->TaskState != ETaskState::None;
	}

	template<typename ReturnType>
	inline FASTINLINE bool TTask<ReturnType>::WasSuccessful() const
	{
		return TaskSource && TaskSource->TaskState == ETaskState::Success;
	}

	template<typename ReturnType>
	FASTINLINE bool TTask<ReturnType>::Wait(s32 Timeout) const {
		FPureTimer Timer;

		while (TaskSource) {
			if (TaskSource->StateNotifier.Wait(Timeout) &&
				TaskSource->TaskState != ETaskState::None)
			{
				return true;
			}

			s32 Milliseconds = Timer.GetMilliseconds();
			if (Timeout >= 0)
			{
				if (Milliseconds >= Timeout)
					break;

				if (Milliseconds) {
					Timeout -= Milliseconds;
					if (Timeout < 0)
						Timeout = 0;

					Timer.Reset();
				}
			}
		}
			
		return false;
	}

	template<typename ReturnType>
	FASTINLINE bool TTask<ReturnType>::GetResult(ReturnType & OutResult) const
	{
		if (IsCompleted() && TaskSource->TaskState == ETaskState::Success) {
			OutResult = *TaskSource->ResultValue;
			return true;
		}

		return false;
	}
}

#endif // !__BOXPP_ASYNC_TASK_HPP__