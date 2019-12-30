#include "IpcClient_Unix.hpp"

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

#if BOX_NOT_COMPILED && PLATFORM_WINDOWS
#	include <winsock2.h>

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#	define socklen_t int
#else
#	include <sys/socket.h>
#	include <sys/un.h>
#endif

namespace boxpp
{
	namespace ipc_internals
	{
		FIpcClient_Unix::FIpcClient_Unix(s32 Socket)
			: Socket(Socket), bUnnamed(true)
		{
		}

		FIpcClient_Unix::FIpcClient_Unix(const ansi_t* Name)
			: Socket(-1), bUnnamed(false)
		{
			::memset(SocketName, 0, sizeof(SocketName));
			IPC_MakePipeName(SocketName, Name);
			TryConnect();
		}

		FIpcClient_Unix::FIpcClient_Unix(const wide_t* Name)
			: Socket(-1), bUnnamed(false)
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
				TryConnect();
			}
		}

		bool FIpcClient_Unix::TryConnect()
		{
			if (!IPC_EnsurePipeDirectory() ||
				(Socket = ::socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
			{
				return false;
			}

			sockaddr_un SA = { 0, };
			SA.sun_family = AF_UNIX;
			TNativeString<ansi_t>::Strcpy(SA.sun_path, SocketName);

			if (::connect(Socket, (sockaddr*)&SA, sizeof(SA))) {
				close(Socket);
				Socket = -1;
				return;
			}

			return true;
		}

		ssize_t FIpcClient_Unix::Receive(void* Buffer, size_t Size)
		{
			while (Socket >= 0) {
				ssize_t ReceivedBytes = read(Socket, Buffer, Size);

				if (ReceivedBytes <= 0) {
					if (errno == EINTR)
						continue;

					Close();
				}
				
				return ReceivedBytes;
			}

			return -1;
		}

		ssize_t FIpcClient_Unix::Send(const void* Buffer, size_t Size)
		{
			ssize_t RetVal = -1;

			while (Socket >= 0) {
				ssize_t SentBytes = write(Socket, Buffer, Size);

				if (SentBytes <= 0) {
					if (errno == EINTR)
						continue;

					if (!(!Size && !SentBytes)) {
						RetVal = RetVal < 0 ? 0 : RetVal;
						break;
					}

					Close();

					if (!SentBytes && RetVal > 0) {
						break;
					}

					RetVal = -1;
					break;
				}

				if (Size >= SentBytes) {
					if (RetVal < 0) {
						RetVal = 0;
					}

					Buffer = (u8*)Buffer + SentBytes;
					Size -= SentBytes;
					RetVal += SentBytes;

					if (!SentBytes) {
						break;
					}
				}

				else if (Size < SentBytes) {
					if (RetVal < 0) {
						RetVal = 0;
					}

					RetVal += SentBytes;
					Size = 0;
				}

				if (!Size) {
					if (RetVal <= 0)
						return 0;

					break;
				}
			}

			return RetVal;
		}

		bool FIpcClient_Unix::Close()
		{
			if (Socket >= 0) {
				shutdown(Socket, SHUT_RDWR);
				close(Socket);
				Socket = -1;
				return true;
			}

			return false;
		}


	}
}
#endif