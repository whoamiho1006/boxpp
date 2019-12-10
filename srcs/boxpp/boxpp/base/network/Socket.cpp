#include "Socket.hpp"
#include <boxpp/base/network/SocketLayer.hpp>

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
		: Socket(-1)
	{
		SOCKET s;
	}

	FSocket::FSocket(__TcpType)
		: Socket(-1)
	{
	}

	FSocket::FSocket(__UdpType)
		: Socket(-1)
	{
	}

	FSocket::FSocket(FSocket&& Other)
		: Socket(Other.Socket)
	{
		Other.Socket = -1;
	}

	FSocket::~FSocket()
	{
		if (Socket >= 0) {
			closesocket(Socket);
		}

		Socket = -1;
	}

	FSocket& FSocket::operator=(FSocket&& Other)
	{

		return *this;
	}

	bool FSocket::Close()
	{
		if (Socket >= 0) {
			closesocket(Socket);
			Socket = -1;

			return true;
		}

		return false;
	}
}