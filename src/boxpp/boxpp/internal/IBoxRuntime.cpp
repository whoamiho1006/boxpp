#include <boxpp.hpp>
#include <boxpp/async/Worker.hpp>
#include <boxpp/async/ThreadLocal.hpp>
#include <boxpp/internal/IBoxRuntime.hpp>

#include <impls/Box.hpp>
#include <impls/BoxSuper.hpp>

#include <assert.h>
#include <malloc.h>
#include <string.h>

namespace boxpp_rt 
{
	FBoxRuntime& FBoxRuntime::Get() {
		static FBoxRuntime Instance;
		return Instance;
	}

	FBoxRuntime::FBoxRuntime()
		: Executable(nullptr), bExecRan(false), BoxSuper(nullptr)
	{
		BoxSuper = new boxpp::FBoxSuper();
	}

	FBoxRuntime::~FBoxRuntime()
	{
		assert(BoxSuper);
		delete BoxSuper;
	}

	bool FBoxRuntime::Enter(IBoxRuntime* RT)
	{
		if (RT->GetType() == ERuntimeType::Exe ||
			RT->GetType() == ERuntimeType::ExeW32)
		{
			if (Get().Executable)
				return false;

			Get().Executable = RT;
		}

		return Get().Add(RT);
	}

	bool FBoxRuntime::Leave(IBoxRuntime* RT)
	{
		bool bUnsetExecutable = false;

		if ((RT->GetType() == ERuntimeType::Exe ||
			RT->GetType() == ERuntimeType::ExeW32) &&
			Get().Executable == RT)
		{
			boxpp::TArray<boxpp::async::FWorker*> Workers;

			while (true) {
				if (true) {
					boxpp::FBarriorScope Guard(Get().Barrior);

					if (!Get().Workers) {
						break;
					}

					Workers.Append(Get().Workers);
				}

				// Wait completion of all workers.
				for (boxpp::async::FWorker* Worker : Workers) {
					Worker->WaitExit();
				}

				Workers.Clear();
			}

			bUnsetExecutable = true;
		}

		if (void* Thread = RT->GetRunningThread()) {
			boxpp::async::_::FThreadLocalServer::Get()
				.ShutdownNative(Thread);
		}

		if (bUnsetExecutable) {
			Get().Executable = nullptr;
		}

		return Get().Remove(RT);
	}

	bool FBoxRuntime::Exec(IBoxRuntime* RT)
	{
		if (RT->GetType() == ERuntimeType::Exe ||
			RT->GetType() == ERuntimeType::ExeW32)
		{
			RT->Run();
			return true;
		}

		return false;
	}

	bool FBoxRuntime::Add(IBoxRuntime* RT)
	{
		boxpp::ssize_t Index = 0;

		Barrior.Enter();
		if (Runtimes.IndexOf(RT) >= 0) {
			Barrior.Leave();

			/* ALREADY LOADED. */
			return false;
		}

		if ((Index = Runtimes.Add(RT)) >= 0) {
			Barrior.Leave();

			if (boxpp::FBox* Box = new boxpp::FBox(BoxSuper->ToWeak())) {
				BoxSuper->Register(Box->ToWeak());
				IBoxRuntime::Deployer::Deploy(RT, Box);
				return true;
			}

			Barrior.Enter();
			Runtimes.RemoveAt(Index);
		}

		Barrior.Leave();
		return false;
	}

	bool FBoxRuntime::Remove(IBoxRuntime* RT)
	{
		boxpp::ssize_t Index = 0;

		Barrior.Enter();
		if ((Index = Runtimes.IndexOf(RT)) >= 0 &&
			Runtimes.RemoveAt(Index))
		{
			Barrior.Leave();

			if (boxpp::IBox* Box = RT->GetBox()) {
				IBoxRuntime::Deployer::Deploy(RT, nullptr);
				BoxSuper->Unregister(Box->ToWeak());
				delete Box;
			}

			return true;
		}

		Barrior.Leave();
		return false;
	}

	/* enter runtime-environment for boxpp. */
	NO_MANGLED BOXPP bool BOX_STDCALL bxEnterRuntime(IBoxRuntime* Runtime) {
		if (Runtime) {
			return FBoxRuntime::Enter(Runtime);
		}

		return false;
	}

	/* leave from runtime-environment for boxpp. */
	NO_MANGLED BOXPP bool BOX_STDCALL bxLeaveRuntime(IBoxRuntime* Runtime) {
		if (Runtime) {
			return FBoxRuntime::Leave(Runtime);
		}

		return false;
	}

	/* exec boxpp. */
	NO_MANGLED BOXPP bool BOX_STDCALL bxExecRuntime(IBoxRuntime* Runtime) {
		if (Runtime) {
			return FBoxRuntime::Exec(Runtime);
		}

		return false;
	}
}
