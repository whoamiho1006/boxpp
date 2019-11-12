#include <boxpp.hpp>
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
		if (RT->GetType() == ERuntimeType::Exe ||
			RT->GetType() == ERuntimeType::ExeW32)
		{
			if (Get().Executable != RT)
				return Get().Remove(RT);

			// Wait.
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
		if (Runtimes.Find(RT) >= 0) {
			Barrior.Leave();

			/* ALREADY LOADED. */
			return false;
		}

		if ((Index = Runtimes.Add(RT)) >= 0) {
			Barrior.Leave();

			if (boxpp::FBox* Box = new boxpp::FBox(BoxSuper)) {
				BoxSuper->Register(Box);
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
		if ((Index = Runtimes.Find(RT)) >= 0 && 
			Runtimes.RemoveAt(Index))
		{
			Barrior.Leave();

			if (boxpp::IBox* Box = RT->GetBox()) {
				IBoxRuntime::Deployer::Deploy(RT, nullptr);
				BoxSuper->Unregister(Box);
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
