#include "IpcServer_Unix.hpp"

#if PLATFORM_UNIX_KIND
#include "IpcClient_Unix.hpp"
#include <boxpp/base/opacities/posix.hpp>

#include <boxpp/base/systems/AtomicBarrior.hpp>
#include <boxpp/base/tpls/strings/String.hpp>
#include <sys/stat.h>
#include <sys/types.h>

#define IPC_IMPL_INCLUDES 1
#include "../windows/IpcMakePipeName.hpp"
#include "IpcEnsureDirectory.hpp"
#undef IPC_IMPL_INCLUDES

#if BOX_NOT_COMPILED
#	include <winsock2.h>
#	define socklen_t int
#else
#	include <sys/socket.h>
#	include <sys/un.h>
#endif

namespace boxpp
{
	namespace ipc_internals
	{		
		FIpcServer_Unix::FIpcServer_Unix(const ansi_t* Name)
			: Socket(-1), bNameBusy(false)
		{
			::memset(SocketName, 0, sizeof(SocketName));
			IPC_MakePipeName(SocketName, Name);
			ArmSocket();
		}

		FIpcServer_Unix::FIpcServer_Unix(const wide_t* Name)
			: Socket(-1), bNameBusy(false)
		{
			bool bNameReady = false;
			static TStringConverter<ansi_t, wide_t> Converter;
			static FAtomicBarrior Atomic;

			::memset(SocketName, 0, sizeof(SocketName));

			{
				FAtomicScope Guard(Atomic);
				Converter.Reset(Name);
				if (Converter) {
					TNativeString<ansi_t>::Strcpy(SocketName, 
						Converter.GetConvertedString());

					bNameReady = true;
				}
			}

			if (bNameReady) {
				ArmSocket();
			}
		}

		FIpcServer_Unix::~FIpcServer_Unix()
		{
			if (Socket >= 0) {
				close(Socket);
			}

			Socket = -1;
		}

		void FIpcServer_Unix::ArmSocket()
		{
			if (!IPC_EnsurePipeDirectory()) {
				bNameBusy = true;
				return;
			}

			bNameBusy = true;

			if ((Socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
				return;
			}

			sockaddr_un SA = { 0, };
			SA.sun_family = AF_UNIX;
			TNativeString<ansi_t>::Strcpy(SA.sun_path, SocketName);

			if (::bind(Socket, (sockaddr*)&SA, sizeof(SA))) {
				close(Socket);
				Socket = -1;
				return;
			}

			bNameBusy = false;
			if (::listen(Socket, 10)) {
				close(Socket);
				Socket = -1;
				return;
			}
		}

		FIpcClient_Unix* FIpcServer_Unix::Accept()
		{
			FIpcClient_Unix* RetVal = nullptr;

			while (Socket >= 0) {
				sockaddr_un SA = { 0, };
				socklen_t SockLen = sizeof(SA);

				s32 Newbie = ::accept(Socket, (sockaddr*)&SA, &SockLen);

				if (Newbie >= 0) {
					RetVal = new FIpcClient_Unix(Newbie);
					break;
				}
			}

			return RetVal;
		}
	}
}
#endif