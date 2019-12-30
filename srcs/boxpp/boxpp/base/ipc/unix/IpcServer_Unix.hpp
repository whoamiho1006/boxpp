#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#if PLATFORM_UNIX_KIND
#include <boxpp/base/opacities/posix.hpp>

namespace boxpp
{
	namespace ipc_internals
	{
		class FIpcClient_Unix;
		class FIpcServer_Unix
		{
		public:
			FIpcServer_Unix(const ansi_t* Name);
			FIpcServer_Unix(const wide_t* Name);
			~FIpcServer_Unix();

		public:
			FASTINLINE bool IsBusy() const { return bNameBusy; }
			FASTINLINE bool HasError() const { return Socket < 0; }

		public:
			s32 Socket;
			bool bNameBusy;
			ansi_t SocketName[256];

		private:
			void ArmSocket();

		public:
			FIpcClient_Unix* Accept();
		};
	}
}
#endif