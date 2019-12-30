#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#if PLATFORM_WINDOWS
#include <boxpp/base/opacities/windows.hpp>
#include <boxpp/base/systems/Barrior.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

namespace boxpp
{
	namespace ipc_internals
	{
		class FIpcClient_Windows {
		protected:
			FIpcClient_Windows(void* hPipe);

		public:
			FIpcClient_Windows(const ansi_t* Name);
			FIpcClient_Windows(const wide_t* Name);
			virtual ~FIpcClient_Windows() { Close(); }

		public:
			FASTINLINE void* GetPipe() const { return hPipe; }
			bool IsBroken() const;

		protected:
			void* hPipe;

			bool bAnsiName, bUnnamed;
			union {
				ansi_t PipeNameA[256];
				wide_t PipeNameW[256];
			};

		private:
			bool TryConnect();

		public:
			ssize_t Receive(void* Buffer, size_t Size);
			ssize_t Send(const void* Buffer, size_t Size);

		public:
			virtual bool Close();
		};
	}

}
#endif