#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#if PLATFORM_UNIX_KIND
namespace boxpp
{
	namespace ipc_internals
	{
		class FIpcServer_Unix;
		class FIpcClient_Unix
		{
			friend class FIpcServer_Unix;

		protected:
			FIpcClient_Unix(s32 Socket);

		public:
			FIpcClient_Unix(const ansi_t* Name);
			FIpcClient_Unix(const wide_t* Name);
			~FIpcClient_Unix() { Close(); }

		public:
			FASTINLINE bool IsBroken() const { return Socket < 0; }

		public:
			s32 Socket;
			bool bUnnamed;
			ansi_t SocketName[256];

		private:
			bool TryConnect();

		public:
			ssize_t Receive(void* Buffer, size_t Size);
			ssize_t Send(const void* Buffer, size_t Size);

		public:
			bool Close();
		};
	}
}
#endif