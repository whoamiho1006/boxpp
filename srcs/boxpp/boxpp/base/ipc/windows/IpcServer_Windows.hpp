#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#if PLATFORM_WINDOWS
#define BOXPP_IPC_PIPEWIDTH		2048

#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/systems/Barrior.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>
#include <boxpp/base/tpls/containers/LinkedList.hpp>

namespace boxpp
{
	namespace ipc_internals
	{
		class FIpcClient_Windows;
		class FIpcContext_Windows;

		class FIpcServer_Windows
		{
			friend class FIpcContext_Windows;

		public:
			FIpcServer_Windows(const ansi_t* Name, size_t PipeWidth = BOXPP_IPC_PIPEWIDTH);
			FIpcServer_Windows(const wide_t* Name, size_t PipeWidth = BOXPP_IPC_PIPEWIDTH);
			~FIpcServer_Windows();

		public:
			FASTINLINE bool IsBusy() const { return bPipeBusy; }
			FASTINLINE bool HasError() const { return bPipeError; }

		private:
			size_t PipeWidth;
			void* hCurrentPipe;
			bool bPipeBusy, bPipeError;
			
			bool bAnsiName;
			union {
				ansi_t PipeNameA[256];
				wide_t PipeNameW[256];
			};

		public:
			FIpcClient_Windows* Accept();

		private:
			bool RearmPipe();

		};
	}

}
#endif