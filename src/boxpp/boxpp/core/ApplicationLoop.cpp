#include "ApplicationLoop.hpp"

#include <boxpp/core/Threading.hpp>

namespace boxpp {

	bool FApplicationLoop::Register(const TSharedPtr<IApplicationLoop>& Loop) {
		FBarriorScope Guard(Barrior);

		if (Loop && !bLoopKilled && !Subloops.Contains(Loop)) {
			IApplicationLoop::Deployer::Deploy(Loop.GetRaw(), Engine);
			Subloops.Add(Loop);

			Loop->Init();
			return true;
		}

		return false;
	}

	bool FApplicationLoop::Unregister(const TSharedPtr<IApplicationLoop>& Loop) {
		FBarriorScope Guard(Barrior);

		if (Subloops.Contains(Loop)) {
			Subloops.Remove(Loop);
			return true;
		}

		return false;
	}

	void FApplicationLoop::PrepareCurrent()
	{
		CurrentLoops.Clear(true);

		if (Subloops.GetSize() > 0)
		{
			FBarriorScope Guard(Barrior);
			CurrentLoops.Append(Subloops);
		}
	}

	bool FApplicationLoop::CanStep() const
	{
		return bKeepRunning && !bLoopKilled;
	}

	void FApplicationLoop::Init()
	{
		bKeepRunning = true;
	}

	void FApplicationLoop::Step()
	{
		if (!bLoopKilled) {
			s32 LoopCounts = 0;

			PrepareCurrent();
			bStepping = true;

			if (CurrentLoops.GetSize() > 0)
			{
				for (TSharedPtr<IApplicationLoop>& Loop : CurrentLoops) {
					if (Loop && Loop->CanStep())
					{
						++LoopCounts;
						Loop->Step();
					}
				}
			}

			else {
				FThreading::YieldOnce();
			}

			if (!LoopCounts) {
				bKeepRunning = false;
			}

			bStepping = false;
		}
	}

	void FApplicationLoop::Exit()
	{
		if (bLoopKilled)
			return;

		if (bStepping) {
			bKeepRunning = false;
			return;
		}

		PrepareCurrent();
		bLoopKilled = true;
		bKeepRunning = false;

		if (CurrentLoops.GetSize() > 0)
		{
			for (TSharedPtr<IApplicationLoop>& Loop : CurrentLoops) {
				if (Loop)
				{
					Loop->Exit();
				}
			}
		}
	}
}