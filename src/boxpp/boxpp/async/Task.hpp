#ifndef __BOXPP_ASYNC_TASK_HPP__
#define __BOXPP_ASYNC_TASK_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Event.hpp>
#include <boxpp/sharedptr/SharedPtr.hpp>

namespace boxpp {
	enum class ETaskState
	{
		/* Not started and no changes being. */
		None = 0,

		/* Task completed successfully. */
		Success,

		/* Task completed, but failed. */
		Failure
	};

	namespace async {
		template<typename ReturnType>
		class TTaskSource;

		template<typename ReturnType>
		using TTaskSourcePtr = TSharedPtr<TTaskSource<ReturnType>, ESharedMode::Safe>;
	}

	template<typename ReturnType>
	class TTask
	{
		template<typename>
		friend class async::TTaskSource;

	public:
		TTask(nullptr_t = nullptr) { }
		TTask(const TTask<ReturnType>& Task) : TaskSource(Task.TaskSource) { }
		TTask(TTask<ReturnType>&& Task) : TaskSource(TMovable<async::TTaskSourcePtr<ReturnType>>::Movable(Task.TaskSource)) { }
		TTask(const async::TTaskSourcePtr<ReturnType>& TaskSource)
			: TaskSource(TaskSource)
		{
		}

	public:
		~TTask() { }

	private:
		async::TTaskSourcePtr<ReturnType> TaskSource;

	public:
		FASTINLINE bool IsValid() const { return TaskSource; }

	public:
		FASTINLINE operator bool() const { return IsValid(); }
		FASTINLINE bool operator !() const { return !IsValid(); }

	public:
		FASTINLINE TTask<ReturnType>& operator =(nullptr_t) {
			TaskSource = nullptr;
			return *this;
		}

		FASTINLINE TTask<ReturnType>& operator =(const TTask<ReturnType>& Task) {
			TaskSource = Task.TaskSource;
			return *this;
		}

		FASTINLINE TTask<ReturnType>& operator =(TTask<ReturnType>& Task) {
			TaskSource = TMovable<async::TTaskSourcePtr<ReturnType>>::Movable(Task.TaskSource);
			return *this;
		}

		FASTINLINE TTask<ReturnType>& operator =(const async::TTaskSourcePtr<ReturnType>& TaskSource) {
			this->TaskSource = TaskSource;
			return *this;
		}

	public:
		/*	Determines this task completed or not. 
			(Implementation: TaskSource.hpp) */
		FASTINLINE bool IsCompleted() const;

		/*	Determines this task was successful or not.
			(Implementation: TaskSource.hpp)  */
		FASTINLINE bool WasSuccessful() const;

		/*	Wait completion during timeout. 
			(Implementation: TaskSource.hpp) */
		FASTINLINE bool Wait(s32 Timeout = -1) const;

	private:
		/*	Get result if possible.
			(Implementation: TaskSource.hpp)  */
		FASTINLINE bool GetResult(ReturnType& OutResult) const;

	public:
		/*	Wait completion and gets the result value. */
		FASTINLINE bool Wait(ReturnType& OutResult, s32 Timeout = -1) const {
			if (Wait(Timeout)) {
				return GetResult(OutResult);
			}

			return false;
		}
	};
}

#endif // !__BOXPP_ASYNC_TASK_HPP__