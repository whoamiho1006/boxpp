#pragma once
#if BOX_NOT_COMPILED
#include <boxpp/Base.hpp>
#endif


#if BOX_COMPILE_RUNTIME || BOX_COMPILE_BODY
#ifndef __BOXPP_INTERNAL_IBOXRUNTIME_HPP__
#define __BOXPP_INTERNAL_IBOXRUNTIME_HPP__

#if BOX_COMPILE_BODY
#include <boxpp/async/Barrior.hpp>
#include <boxpp/containers/SortedArray.hpp>

namespace boxpp {
	class FBoxSuper;
#if BOX_COMPILE_BODY
	namespace async {
		class FWorker;
	}
#endif
}
#endif

#include <boxpp/async/Thread.hpp>

namespace boxpp_rt 
{
	enum class ERuntimeType
	{
		/* Executable. */
		Exe		= 0,

		/* Windows App. (WinMain) */
		ExeW32	= PLATFORM_WINDOWS ? 1 : Exe,

		/* Module library. */
		Module = 2
	};

#if BOX_COMPILE_BODY
	class FBoxRuntime;
#endif

	/* Box runtime interface. */
	class IBoxRuntime : public boxpp::async::IRunnable {
	public:
		IBoxRuntime() : Box(nullptr), Arguments(nullptr), ArgumentCount(0) { }
		IBoxRuntime(const boxpp::c8** Args, boxpp::s32 Argc) 
			: Box(nullptr), Arguments(Args), ArgumentCount(Argc) { }
		virtual ~IBoxRuntime() { }

	private:
		boxpp::IBox* Box;
		const boxpp::c8** Arguments;
		boxpp::s32 ArgumentCount;
		void* RunningThread;

#if BOX_COMPILE_BODY
	protected:
		friend class FBoxRuntime;
		class Deployer {
			friend class FBoxRuntime;

		protected:
			FASTINLINE static void Deploy(
				IBoxRuntime* Runtime, boxpp::IBox* Box) 
			{
				Runtime->Box = Box;
				Runtime->RunningThread = boxpp::async::FThread::SelfNative();
			}
		};
#endif
	public:
		/* Get type of this runtime. */
		virtual ERuntimeType GetType() const = 0;

		/* Get running thread's native handle. */
		FASTINLINE void* GetRunningThread() const { return RunningThread; }

		/* Get box for this runtime. */
		FASTINLINE boxpp::IBox* GetBox() const { return Box; }

		/* Get count of execution arguments. */
		FASTINLINE boxpp::s32 GetArgumentCount() const { return ArgumentCount; }

		/* Get count of execution arguments. */
		FASTINLINE const boxpp::c8* GetArgument(boxpp::s32 i) const { 
			return ArgumentCount && Arguments ? Arguments[i] : nullptr; 
		}
		
	public:
		/* Run this runtime if required. */
		virtual void Run() { };

	};

#if BOX_COMPILE_BODY /* FBoxRuntime is only valid for boxpp.dll. */
	class FBoxRuntime {
		friend class boxpp::async::FWorker;

	public:
		static FBoxRuntime& Get();

	private:
		FBoxRuntime();
		~FBoxRuntime();

	private:
		IBoxRuntime* Executable;
		boxpp::FBarrior Barrior;
		boxpp::FBoxSuper* BoxSuper;
		boxpp::TSortedArray<IBoxRuntime*> Runtimes;
		boxpp::TSortedArray<boxpp::async::FWorker*> Workers;

	private:
		bool bExecRan;
		
	public:
		static bool Enter(IBoxRuntime* RT);
		static bool Leave(IBoxRuntime* RT);

	public:
		static bool Exec(IBoxRuntime* RT);

	public:
		FASTINLINE static void* GetExeThread() {
			if (IBoxRuntime* Runtime = Get().Executable)
				return Runtime->GetRunningThread();

			return nullptr;
		}

	private:
		bool Add(IBoxRuntime* RT);
		bool Remove(IBoxRuntime* RT);

	protected:
		FASTINLINE void RegisterWorker(boxpp::async::FWorker* Worker) {
			boxpp::FBarriorScope Guard(Barrior);
			Workers.AddUnique(Worker);
		}

		FASTINLINE void UnregisterWorker(boxpp::async::FWorker* Worker) {
			boxpp::FBarriorScope Guard(Barrior);
			Workers.Remove(Worker);
		}
	};
#endif

#if BOX_COMPILE_RUNTIME
	/* Box runtime enter/leave. (don't call if dynamic loaded) */
	NO_MANGLED BOXPP bool BOX_STDCALL bxEnterRuntime(IBoxRuntime* Runtime);
	NO_MANGLED BOXPP bool BOX_STDCALL bxLeaveRuntime(IBoxRuntime* Runtime);
	NO_MANGLED BOXPP bool BOX_STDCALL bxExecRuntime(IBoxRuntime* Runtime);

	/* FAR proc version. */
	typedef bool (BOX_STDCALL* FAR_bxEnterRuntime)(IBoxRuntime* Runtime);
	typedef bool (BOX_STDCALL* FAR_bxLeaveRuntime)(IBoxRuntime* Runtime);
	typedef bool (BOX_STDCALL* FAR_bxExecRuntime)(IBoxRuntime* Runtime);

	/* SYMBOL for finding proc-address. */

#	if PLATFORM_WINDOWS
#		define SYMBOL_bxEnterRuntime_CStyle			"_bxEnterRuntime"
#		define SYMBOL_bxLeaveRuntime_CStyle			"_bxLeaveRuntime"
#		define SYMBOL_bxExecRuntime_CStyle			"_bxExecRuntime"
#	else
#		define SYMBOL_bxEnterRuntime_CStyle			"bxEnterRuntime"
#		define SYMBOL_bxLeaveRuntime_CStyle			"bxLeaveRuntime"
#		define SYMBOL_bxExecRuntime_CStyle			"bxExecRuntime"
#	endif

#	define SYMBOL_bxEnterRuntime				SYMBOL_bxEnterRuntime_CStyle
#	define SYMBOL_bxLeaveRuntime				SYMBOL_bxLeaveRuntime_CStyle
#	define SYMBOL_bxExecRuntime					SYMBOL_bxExecRuntime_CStyle
#	define SYMBOL_bxEnterRuntime_Mangled		SYMBOL_bxEnterRuntime_CStyle "@4"
#	define SYMBOL_bxLeaveRuntime_Mangled		SYMBOL_bxLeaveRuntime_CStyle "@4"
#	define SYMBOL_bxExecRuntime_Mangled			SYMBOL_bxExecRuntime_CStyle "@4"

#	define NAME_bxLibrary			"boxpp"

	/* SHARED for expecting shared-library.*/
#	define SHARED_bxLibrary				\
	NAME_bxLibrary "." PLATFORM_EXT_SHARED

#	define SHARED_bxVersionedLibrary	\
	NAME_bxLibrary "-" BOXPP_VERSION_STRING "." PLATFORM_EXT_SHARED


#endif
}

#endif // !__BOXPP_INTERNAL_IBOXRUNTIME_HPP__
#endif
