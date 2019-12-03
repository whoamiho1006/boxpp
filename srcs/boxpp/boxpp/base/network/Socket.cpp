#include "Socket.hpp"

#if PLATFORM_WINDOWS
#pragma comment(lib, "ws2_32.lib")
#endif

namespace boxpp {
#if PLATFORM_WINDOWS
	class FWSAInitialize
	{
	public:
		static FWSAInitialize& Get() {
			static FWSAInitialize _Obj;
			return _Obj;
		}

	private:
		w32_compat::WSADATA wsaData;
		
	private:
		FWSAInitialize() {
			w32_compat::WSAStartup(2 | (2 << 8), &wsaData);
		}

		~FWSAInitialize() {
			w32_compat::WSACleanup();
		}
	};
#endif

	FSocket::FSocket()
		: Socket(-1)
	{
		FWSAInitialize::Get();
	}

	FSocket::FSocket(__TcpType)
		: Socket(-1)
	{
		FWSAInitialize::Get();
	}

	FSocket::FSocket(__UdpType)
		: Socket(-1)
	{
		FWSAInitialize::Get();
	}

	FSocket::FSocket(FSocket&& Other)
		: Socket(-1)
	{
		FWSAInitialize::Get();
	}

	FSocket::~FSocket()
	{
	}

	FSocket& FSocket::operator=(FSocket&& Other)
	{
		return *this;
	}
}