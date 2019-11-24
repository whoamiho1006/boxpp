#include "Engine.hpp"
#include <boxpp/core/Threading.hpp>
#include <boxpp/core/EngineLoop.hpp>

namespace boxpp {

	FEngine::~FEngine()
	{
	}

	void FEngine::Launch()
	{
		TArray<TSharedPtr<IEngineLoop>> Loops;

		while (KeepRunningLoop())
		{
			s32 Counts = 0;

			if (true) {
				FBarriorScope Guard(Barrior);
				EngineLoops.Clear(true);
				Loops.Append(EngineLoops);
			}

			for (TSharedPtr<IEngineLoop>& Loop : Loops) {
				if (Loop && Loop->CanStep())
				{
					++Counts;
					Loop->Step();
				}
			}

			if (!Counts)
			{
				FThreading::YieldOnce();
			}
		}

		if (true) {
			FBarriorScope Guard(Barrior);
			EngineLoops.Clear(true);
			Loops.Append(EngineLoops);
		}

		for (TSharedPtr<IEngineLoop>& Loop : Loops) {
			if (Loop)
			{
				Loop->Exit();
			}
		}
	}

	bool FEngine::PreInitialize()
	{
		return false;
	}

	bool FEngine::PostInitialize()
	{
		return false;
	}

	bool FEngine::PreFinalize()
	{
		return false;
	}

	bool FEngine::PostFinalize()
	{
		return false;
	}

	bool FEngine::PreConfigure()
	{
		return false;
	}

	bool FEngine::PostConfigure()
	{
		return false;
	}

	void FEngine::UnloadConfigurations()
	{
	}

}