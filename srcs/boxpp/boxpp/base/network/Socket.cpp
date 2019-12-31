#include "Socket.hpp"
#include <boxpp/base/network/SocketLayer.hpp>
#include <boxpp/base/tpls/pools/Mempool.hpp>

#if PLATFORM_WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>

#define closesocket(x) close(x)
#endif
#endif

namespace boxpp {
	FSocket::FSocket()
		: Socket({ 0, })
	{
		Socket.s = -1;
	}

	FSocket::FSocket(EProtocolType Protocol, ESocketType SocketType)
		: Socket(FSocketLayer::Create(Protocol, SocketType))
	{
	}
	
	FSocket::FSocket(FSocket&& Other)
		: Socket(Other.Socket)
	{
		Other.Socket.s = -1;
	}

	FSocket::~FSocket()
	{
		FSocketLayer::Close(Socket);
	}

	FSocket& FSocket::operator=(FSocket&& Other)
	{
		Swap(Socket, Other.Socket);
		return *this;
	}

	bool FSocket::Accept(FSocket& Newbie) {
		if (*this) {
			if (FSocketLayer::IsValid(Newbie.Socket)) {
				FSocketLayer::Close(Newbie.Socket);
			}

			FSocketLayer::Accept(Socket, Newbie.Socket);
			return FSocketLayer::IsValid(Newbie.Socket);
		}

		return false;
	}

	ESocketChannel FSocket::Poll(ESocketChannel Channels, const u32 Timeout) {
		FSocketPoll p;

		p.Socket = Socket;
		p.Events = Channels;

		ssize_t R = FSocketLayer::Poll(&p, 1, Timeout);
		Socket = p.Socket;

		if (R < 0) {
			return ESocketChannel::ESOCK_ERROR;
		}

		return p.Channels;
	}

	ssize_t FSocket::Poll(TArray<FSocket*>& Sockets, ESocketChannel Channels, const u32 Timeout)
	{
		static TMempool<FSocketPoll, 8> Pool;
		TArray<FSocket*> InSockets = Forward<TArray<FSocket*>>(Sockets);

		if (InSockets.GetSize()) {
			FSocketPoll* PollDescs = Pool.Alloc(InSockets.GetSize());
			size_t Multiplier = Sockets.GetMultiplier();

			for (size_t i = 0; i < InSockets.GetSize(); ++i) {
				FSocketPoll& Desc = PollDescs[i];

				Desc.Socket = InSockets[i]->Socket;
				Desc.Events = Channels;
			}

			ssize_t R = FSocketLayer::Poll(PollDescs, InSockets.GetSize(), Timeout);

			if (R >= 0) {
				Sockets.SetMultiplier(2);

				for (size_t i = 0; i < InSockets.GetSize(); ++i) {
					FSocketPoll& Desc = PollDescs[i];

					InSockets[i]->Socket = Desc.Socket;
					if (Desc.Channels != ESOCK_NONE) {
						Sockets.Add(InSockets[i]);
					}
				}
			}

			Sockets.SetMultiplier(Multiplier);
			Sockets.Optimize();
			return R;
		}

		return -1;
	}

	bool FSocket::Close()
	{
		if (FSocketLayer::IsValid(Socket)) {
			FSocketLayer::Close(Socket);
			return true;
		}

		return false;
	}
}