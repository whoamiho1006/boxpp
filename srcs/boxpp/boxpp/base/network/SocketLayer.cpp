#include "SocketLayer.hpp"
#include <boxpp/base/systems/AtomicBarrior.hpp>
#include <boxpp/base/tpls/pools/Mempool.h>

#if PLATFORM_WINDOWS
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#define socklen_t int
#endif

#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <string.h>
#include <unistd.h>

#define closesocket(x) close(x)
#endif
#endif

namespace boxpp {

#if PLATFORM_WINDOWS
	class WSAInit {
	private:
		WSAInit() {
			WSAStartup(MAKEWORD(2, 2), &wWSAData);
		}

		~WSAInit() {
			WSACleanup();
		}

	private:
		WSADATA wWSAData;

	public:
		static WSAInit& Init() {
			static WSAInit _;
			return _;
		}
	};
#endif

#if PLATFORM_WINDOWS
	void FSocketLayer::CheckAndInit()
	{
		WSAInit::Init();
	}
#endif

#if PLATFORM_WINDOWS
	FASTINLINE ESocketError SOCKET_GetErrorCode_Windows(s32* OutRawError) {
		ESocketError Error = ESocketError::Success;
		s32 RawError = -1;

		if (!(!(RawError = WSAGetLastError()))) {
			switch (RawError) {
#if PLATFORM_WINDOWS
			case WSA_IO_PENDING:
				Error = ESocketError::Pending;
				break;

			case WSAEWOULDBLOCK:
				Error = ESocketError::WouldBlock;
				break;

			case WSAEINPROGRESS:
				Error = ESocketError::InProgress;
				break;

			case WSA_IO_INCOMPLETE:
				Error = ESocketError::Incompleted;
				break;

			case WSAEALREADY:
				Error = ESocketError::Already;
				break;

			case WSAECANCELLED:
				Error = ESocketError::Cancelled;
				break;

			case WSAEINTR:
				Error = ESocketError::Interrupted;
				break;

			case WSA_INVALID_HANDLE:
				Error = ESocketError::BadSocket;
				break;

			case WSA_NOT_ENOUGH_MEMORY:
			case WSAETOOMANYREFS:
			case WSAEPROCLIM:
			case WSAEMFILE:
				Error = ESocketError::NoResources;
				break;

			case WSAEFAULT:
				Error = ESocketError::BadAddress;
				break;

			case WSAEDESTADDRREQ:
				Error = ESocketError::NoDestination;
				break;

			case WSAEADDRINUSE:
				Error = ESocketError::AddressInUse;
				break;

			case WSAEADDRNOTAVAIL:
				Error = ESocketError::AddressNotAvailable;
				break;

			case WSAEPROTOTYPE:
				Error = ESocketError::InvalidProtocol;
				break;

			case WSAENOPROTOOPT:
				Error = ESocketError::InvalidProtocolOptions;
				break;

			case WSA_INVALID_PARAMETER:
			case WSAEINVAL:
				Error = ESocketError::InvalidArgument;
				break;

			case WSAEPROTONOSUPPORT:
				Error = ESocketError::NotSupportedProtocol;
				break;

			case WSAESOCKTNOSUPPORT:
				Error = ESocketError::NotSupportedSocket;
				break;

			case WSAEOPNOTSUPP:
				Error = ESocketError::NotSupportedOperation;
				break;

			case WSAEPFNOSUPPORT:
			case WSAEAFNOSUPPORT:
				Error = ESocketError::NotSupportedFamily;
				break;

			case WSAEHOSTDOWN:
				Error = ESocketError::HostDown;
				break;

			case WSAEHOSTUNREACH:
				Error = ESocketError::HostUnreachable;
				break;

			case WSAHOST_NOT_FOUND:
				Error = ESocketError::HostNotFound;
				break;

			case WSAENETDOWN:
				Error = ESocketError::NetworkDown;
				break;

			case WSAENETUNREACH:
				Error = ESocketError::NetworkUnreachable;
				break;

			case WSAECONNABORTED:
				Error = ESocketError::Aborted;
				break;

			case WSAECONNREFUSED:
				Error = ESocketError::Refused;
				break;

			case WSAENOTCONN:
				Error = ESocketError::NotConnected;
				break;

			case WSAENOBUFS:
				Error = ESocketError::NoBuffer;
				break;

			case WSAETIMEDOUT:
				Error = ESocketError::Timedout;
				break;

			case WSAESHUTDOWN:
				Error = ESocketError::Shutdown;
				break;

			case WSAECONNRESET:
				Error = ESocketError::ResetByPeer;
				break;

			case WSAENETRESET:
				Error = ESocketError::ResetByNetwork;
				break;

			case WSAEISCONN:
				Error = ESocketError::ConnectedAlready;
				break;
#endif
			default:
				Error = ESocketError::NotWrapped;
				break;
			}
		}

		if (OutRawError) {
			*OutRawError = RawError;
		}

		return Error;
	}
#endif

	FASTINLINE ESocketError SOCKET_GetErrorCodeFromRaw(s32* RawError) {
		ESocketError Error = ESocketError::Success;
		s32 RawErr = *RawError;

		if (!(!(RawErr))) {
			switch (RawErr) {
#if defined(EAGAIN) && EAGAIN != EWOULDBLOCK
			case EAGAIN:
#endif
			case EWOULDBLOCK:
				Error = ESocketError::WouldBlock;
				break;

			case EINPROGRESS:
				Error = ESocketError::InProgress;
				break;

			case EALREADY:
				Error = ESocketError::Already;
				break;

			case ECANCELED:
				Error = ESocketError::Cancelled;
				break;

			case EINTR:
				Error = ESocketError::Interrupted;
				break;

			case EBADF:
#if defined (EBADFD)
			case EBADFD:
#endif
			case ENOTSOCK:
				Error = ESocketError::BadSocket;
				break;

			case ENOMEM:
			case ENFILE:
			case ENODEV:
			case EMFILE:
			case EBUSY:
			case ENOENT:
#if defined (ETOOMANYREFS)
			case ETOOMANYREFS:
#endif
				Error = ESocketError::NoResources;
				break;

			case EFAULT:
				Error = ESocketError::BadAddress;
				break;

			case EDESTADDRREQ:
				Error = ESocketError::NoDestination;
				break;

			case EADDRINUSE:
				Error = ESocketError::AddressInUse;
				break;

			case EADDRNOTAVAIL:
				Error = ESocketError::AddressNotAvailable;
				break;

			case EPROTOTYPE:
				Error = ESocketError::InvalidProtocol;
				break;

			case ENOPROTOOPT:
				Error = ESocketError::InvalidProtocolOptions;
				break;

			case EINVAL:
				Error = ESocketError::InvalidArgument;
				break;

			case EPROTONOSUPPORT:
#if defined (EUNATCH)
			case EUNATCH:
#endif
				Error = ESocketError::NotSupportedProtocol;
				break;

#if defined (ESOCKTNOSUPPORT)
			case ESOCKTNOSUPPORT:
				Error = ESocketError::NotSupportedSocket;
				break;
#endif

			case ENOTSUP:
			case EOPNOTSUPP:
				Error = ESocketError::NotSupportedOperation;
				break;

#if defined(EPFNOSUPPORT) && (EPFNOSUPPORT != EAFNOSUPPORT)
			case EPFNOSUPPORT:
#endif
			case EAFNOSUPPORT:
				Error = ESocketError::NotSupportedFamily;
				break;

#if defined(EHOSTDOWN)
			case EHOSTDOWN:
				Error = ESocketError::HostDown;
				break;
#endif

			case EHOSTUNREACH:
				Error = ESocketError::HostUnreachable;
				break;

			case ENETDOWN:
				Error = ESocketError::NetworkDown;
				break;

			case ENETUNREACH:
				Error = ESocketError::NetworkUnreachable;
				break;

			case ECONNABORTED:
				Error = ESocketError::Aborted;
				break;

			case ECONNREFUSED:
				Error = ESocketError::Refused;
				break;

			case ENOTCONN:
				Error = ESocketError::NotConnected;
				break;

			case ENOBUFS:
				Error = ESocketError::NoBuffer;
				break;

			case ETIMEDOUT:
				Error = ESocketError::Timedout;
				break;

#if defined(ESHUTDOWN)
			case ESHUTDOWN:
				Error = ESocketError::Shutdown;
				break;
#endif

			case ECONNRESET:
				Error = ESocketError::ResetByPeer;
				break;

			case ENETRESET:
				Error = ESocketError::ResetByNetwork;
				break;

			case EISCONN:
				Error = ESocketError::ConnectedAlready;
				break;

			default:
#if PLATFORM_WINDOWS
				Error = SOCKET_GetErrorCode_Windows(RawError);
#else
				Error = ESocketError::NotWrapped;
#endif
				break;
			}
		}

		return Error;
	}

	FASTINLINE ESocketError SOCKET_GetErrorCode(s32 s, s32* OutRawError) {
		s32 Err = 0;
		socklen_t ErrLen = sizeof(Err);

		if (::getsockopt(s, SOL_SOCKET, SO_ERROR,
			(char*)&Err, (int*)&ErrLen) == 0)
		{
			ESocketError Error = SOCKET_GetErrorCodeFromRaw(&Err);

			if (OutRawError) {
				*OutRawError = Err;
			}

			return Error;
		}

		return ESocketError::BadSocket;
	}

	FRawSocket FSocketLayer::Create(EProtocolType ProtocolType, ESocketType SocketType) {
		int AfVal = AF_INET, SkType = SOCK_STREAM;
		FRawSocket Socket = { 0, };

		switch (ProtocolType)
		{
		case EProtocolType::Inet: AfVal = AF_INET; break;
		case EProtocolType::Inet6: AfVal = AF_INET6; break;
		default:
			Socket.s = -1;
			Socket.err = ESocketError::NotSupportedProtocol;
			return Socket;
		}

		switch (SocketType)
		{
		case ESocketType::Tcp: SkType = SOCK_STREAM; break;
		case ESocketType::Udp: SkType = SOCK_DGRAM; break;
		default:
			Socket.s = -1;
			Socket.err = ESocketError::NotSupportedSocket;
			return Socket;
		}

#if PLATFORM_WINDOWS
		WSAInit::Init();
#endif

		Socket.type = SkType;
		Socket.family = AfVal;
		Socket.err = ESocketError::Success;
		Socket.err_raw = 0;

#if PLATFORM_WINDOWS
		SOCKET s = ::socket(AfVal, SkType, 0);
		if (s == INVALID_SOCKET) {
			Socket.s = -1;
			Socket.err = ESocketError::BadSocket;
			return Socket;
		}

		Socket.s = s32(s);
#endif 
#if PLATFORM_POSIX
		Socket.s = ::socket(AfVal, SkType, 0);
#endif

		if (Socket.s <= 0) {
			Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
		}

		return Socket;
	}

	void FSocketLayer::Close(FRawSocket& Socket)
	{
		if (IsValid(Socket)) {
			::closesocket(Socket.s);
		}

		Socket.s = -1;
	}

	ESocketError FSocketLayer::ResolveErrorCode(const FRawSocket& Socket) {
		return SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
	}

	void FSocketLayer::CleanErrorCode(const FRawSocket& Socket)
	{
		s32 Err = 0;

		if (IsValid(Socket)) {
			Socket.err = ESocketError::Success;
			Socket.err_raw = 0;

			::setsockopt(Socket.s, SOL_SOCKET, 
				SO_ERROR, (char*)&Err, sizeof(Err));

#if PLATFORM_WINDOWS
			WSASetLastError(0);
#endif
		}
	}

#if PLATFORM_WINDOWS
	using ioctl_bool_t = u_long;
#else
	using ioctl_bool_t = s32;
#endif

	bool FSocketLayer::GetSockName(const FRawSocket& Socket, FIPAddress& Address, s32& Port)
	{
		sockaddr_storage SASt;
		socklen_t SASLen = sizeof(SASt);

		if (!getsockname(Socket.s, (sockaddr*)&SASt, &SASLen)) {
			if (SASt.ss_family == AF_INET) {
				sockaddr_in* SA = (sockaddr_in*)&SASt;

				u8* AddrBytes = (u8*)(&(SA->sin_addr));

				Address.SetByteAt(0, AddrBytes[0]);
				Address.SetByteAt(1, AddrBytes[1]);
				Address.SetByteAt(2, AddrBytes[2]);
				Address.SetByteAt(3, AddrBytes[3]);

				Port = ntohs(SA->sin_port);
				return true;
			}
		}

		return false;
	}

	bool FSocketLayer::GetSockName(const FRawSocket& Socket, FIPAddressV6& Address, s32& Port)
	{
		sockaddr_storage SASt;
		socklen_t SASLen = sizeof(SASt);

		if (!getsockname(Socket.s, (sockaddr*)&SASt, &SASLen)) {
			if (SASt.ss_family == AF_INET6) {
				sockaddr_in6* SA = (sockaddr_in6*)&SASt;

				u8* AddrBytes = (u8*)(&(SA->sin6_addr));

				for (s8 i = 0; i < 4; ++i) {
					Address.SetByteAt(i * 4 + 0, AddrBytes[i * 4 + 0]);
					Address.SetByteAt(i * 4 + 1, AddrBytes[i * 4 + 1]);
					Address.SetByteAt(i * 4 + 2, AddrBytes[i * 4 + 2]);
					Address.SetByteAt(i * 4 + 3, AddrBytes[i * 4 + 3]);
				}

				Port = ntohs(SA->sin6_port);
				return true;
			}
		}

		return false;
	}

	bool FSocketLayer::IsNonblock(const FRawSocket& Socket)
	{
		if (IsValid(Socket)) {
			s32 R = 0;

#if PLATFORM_WINDOWS
			char b = 0;

			CleanErrorCode(Socket);
			R = ::recv(Socket.s, &b, 0, 0);

			/*
				There is no official-way to determine the socket is non blocking or not.
				https://stackoverflow.com/questions/5489562/in-win32-is-there-a-way-to-test-if-a-socket-is-non-blocking

				Summary:
				Previously, WSAIsBlocking API supported, but, now-days not.
				So, Call recv for zero-byte and test the return value is zero or not.
				~~ So, we should use tricky way for checking it correctly.

				Ret-Val,	0:				Blocking socket.
							WSAEWOULDBLOCK:	Non-blocking socket.
				Else,		~~~:			Failure by socket-error.
			*/

			if (!R)
				return false;

			else if (R == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
				return true;
#endif
#if PLATFORM_POSIX
			R = fcntl(Socket.s, F_GETFL, 0);
			if (R != -1 && ((R & O_NONBLOCK) == O_NONBLOCK))
				return true;
#endif
		}

		return false;
	}

	ssize_t FSocketLayer::SetNonblock(const FRawSocket & Socket, bool Value)
	{
		if (IsValid(Socket)) {
			ioctl_bool_t Mode = Value ? 0 : 1;
			ssize_t RetVal = 0;

#if PLATFORM_WINDOWS
			RetVal = ioctlsocket(Socket.s, FIONBIO, &Mode);
#endif
#if PLATFORM_POSIX
			Mode = fcntl(Socket.s, F_GETFL, 0);
			if (Mode == -1)
				RetVal = -1;

			else {
				Mode = (Value ? (Mode | O_NONBLOCK) : (Mode & ~O_NONBLOCK));
				if (fcntl(Socket.s, F_SETFL, Mode) == -1) {
					RetVal = -1;
				}
			}
#endif
			return RetVal;
		}

		return -1;
	}

	bool FSocketLayer::IsNoDelay(const FRawSocket& Socket)
	{
		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			s32 Value = 0, ValLen = sizeof(s32);

			if (::getsockopt(Socket.s, IPPROTO_TCP, TCP_NODELAY,
				(char*)&Value, (int*)&ValLen) == 0)
			{
				return Value != 0;
			}
		}

		return false;
	}

	ssize_t FSocketLayer::SetNoDelay(const FRawSocket & Socket, bool Value)
	{
		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			s32 Yes = 1;

			return ::setsockopt(Socket.s, IPPROTO_TCP,
				TCP_NODELAY, (char*)&Yes, sizeof(Yes));
		}

		return -1;
	}

	s32 FSocketLayer::GetRecvTimeout(const FRawSocket & Socket)
	{
		if (IsValid(Socket)) {
			timeval tv;
			s32 ValLen = sizeof(tv);

			if (::getsockopt(Socket.s, SOL_SOCKET, SO_RCVTIMEO,
				(char*)&tv, (int*)&ValLen) == 0)
			{
				return ((s64)tv.tv_sec) * 1000 + ((s64)tv.tv_usec) / 1000;
			}
		}

		return -1;
	}

	s32 FSocketLayer::GetSendTimeout(const FRawSocket & Socket)
	{
		if (IsValid(Socket)) {
			timeval tv;
			s32 ValLen = sizeof(tv);

			if (::getsockopt(Socket.s, SOL_SOCKET, SO_SNDTIMEO,
				(char*)&tv, (int*)&ValLen) == 0)
			{
				return ((s64)tv.tv_sec) * 1000 + ((s64)tv.tv_usec) / 1000;
			}
		}

		return -1;
	}

	ssize_t FSocketLayer::SetRecvTimeout(const FRawSocket& Socket, s32 Timeout)
	{
		if (IsValid(Socket)) {
			timeval tv; 

			tv.tv_sec = Timeout / 1000;
			tv.tv_usec = (Timeout % 1000) * 1000;

			return ::setsockopt(Socket.s, SOL_SOCKET,
				SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
		}
		
		return -1;
	}

	ssize_t FSocketLayer::SetSendTimeout(const FRawSocket& Socket, s32 Timeout)
	{
		if (IsValid(Socket)) {
			timeval tv;

			tv.tv_sec = Timeout / 1000;
			tv.tv_usec = (Timeout % 1000) * 1000;

			return ::setsockopt(Socket.s, SOL_SOCKET,
				SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
		}

		return -1;
	}

	ssize_t FSocketLayer::Bind(const FRawSocket& Socket, const FIPAddress& Address, s32 Port)
	{
		if (IsValid(Socket)) {
			sockaddr_in SockAddr = { 0, };
			u8* AddrBytes = (u8*)(&SockAddr.sin_addr);
			ssize_t R = -1;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			SockAddr.sin_family = Socket.family;
			SockAddr.sin_port = htons(u16(Port));

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			AddrBytes[0] = Address.GetByteAt(0);
			AddrBytes[1] = Address.GetByteAt(1);
			AddrBytes[2] = Address.GetByteAt(2);
			AddrBytes[3] = Address.GetByteAt(3);

			CleanErrorCode(Socket);

			if ((R = ::bind(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	FASTINLINE void SOCKET_IPv6ToSA6(sockaddr_in6& OutSA, const FIPAddressV6& Address) {
		u8* AddrBytes = (u8*)(&OutSA.sin6_addr);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

		OutSA.sin6_family = AF_INET6;
		OutSA.sin6_scope_id = Address.GetScopeId();

#ifdef _MSC_VER
#pragma warning(pop)
#endif
		for (s8 i = 0; i < 4; ++i) {
			AddrBytes[i * 4 + 0] = Address.GetByteAt(i * 4 + 0);
			AddrBytes[i * 4 + 1] = Address.GetByteAt(i * 4 + 1);
			AddrBytes[i * 4 + 2] = Address.GetByteAt(i * 4 + 2);
			AddrBytes[i * 4 + 3] = Address.GetByteAt(i * 4 + 3);
		}
	}

	ssize_t FSocketLayer::Bind(const FRawSocket& Socket, const FIPAddressV6& Address, s32 Port)
	{
		if (IsValid(Socket)) {
			sockaddr_in6 SockAddr = { 0, };
			ssize_t R = -1;

			SOCKET_IPv6ToSA6(SockAddr, Address);
			SockAddr.sin6_port = htons(u16(Port));

			CleanErrorCode(Socket);

			if ((R = ::bind(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Listen(const FRawSocket& Socket, s32 Backlog)
	{
		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			ssize_t R = -1;

			CleanErrorCode(Socket);

			if ((R = ::listen(Socket.s, Backlog)) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Accept(const FRawSocket& Socket, FRawSocket& Newbie)
	{
		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			ssize_t R = -1;

			sockaddr_in SockAddr;
			socklen_t SockLen = sizeof(SockAddr);

			Newbie.s = -1;
			Newbie.err = ESocketError::WouldBlock;

			CleanErrorCode(Socket);

			if ((R = ::accept(Socket.s, (sockaddr*)&SockAddr, &SockLen)) >= 0) {
				Newbie.s = s32(R);
				Newbie.err = ESocketError::Success;
				Newbie.err_raw = 0;
				Newbie.type = Socket.type;
				Newbie.family = Socket.family;
			}

			else {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}
			
			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Connect(const FRawSocket& Socket, const FIPAddress& Address, s32 Port)
	{
		if (IsValid(Socket)) {
			sockaddr_in SockAddr = { 0, };
			u8* AddrBytes = (u8*)(&SockAddr.sin_addr);
			bool RNonBlk = false;
			ssize_t R = -1;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			SockAddr.sin_family = Socket.family;
			SockAddr.sin_port = htons(Port);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			AddrBytes[0] = Address.GetByteAt(0);
			AddrBytes[1] = Address.GetByteAt(1);
			AddrBytes[2] = Address.GetByteAt(2);
			AddrBytes[3] = Address.GetByteAt(3);

			if (IsNonblock(Socket)) {
				RNonBlk = true;
				SetNonblock(Socket, false);
			}

			CleanErrorCode(Socket);

			if ((R = ::connect(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			if (RNonBlk) {
				SetNonblock(Socket, true);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Connect(const FRawSocket & Socket, const FIPAddressV6 & Address, s32 Port)
	{
		if (IsValid(Socket)) {
			sockaddr_in6 SockAddr = { 0, };
			bool RNonBlk = false;
			ssize_t R = -1;

			SOCKET_IPv6ToSA6(SockAddr, Address);
			SockAddr.sin6_port = htons(u16(Port));

			if (IsNonblock(Socket)) {
				RNonBlk = true;
				SetNonblock(Socket, false);
			}

			CleanErrorCode(Socket);

			if ((R = ::connect(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			if (RNonBlk) {
				SetNonblock(Socket, true);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Connect(const FRawSocket& Socket, const FIPAddress& Address, s32 Port, s32 Timeout)
	{
		if (Timeout < 0)
			return Connect(Socket, Address, Port);

		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			bool RNonblk = IsNonblock(Socket);
			sockaddr_in SockAddr = { 0, };
			u8* AddrBytes = (u8*)(&SockAddr.sin_addr);
			ssize_t R = -1, TrySelect = 1;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			SockAddr.sin_family = Socket.family;
			SockAddr.sin_port = htons(Port);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			AddrBytes[0] = Address.GetByteAt(0);
			AddrBytes[1] = Address.GetByteAt(1);
			AddrBytes[2] = Address.GetByteAt(2);
			AddrBytes[3] = Address.GetByteAt(3);

			if (!RNonblk) {
				SetNonblock(Socket, true);
			}

			CleanErrorCode(Socket);

			if ((R = ::connect(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);

				if (Socket.err != ESocketError::InProgress &&
					Socket.err != ESocketError::WouldBlock &&
					Socket.err != ESocketError::Pending) 
				{
					TrySelect = 0;
				}
			}

			if (TrySelect) {
				fd_set WriteFDSet, ErrorFDSet;
				timeval tv;

				tv.tv_sec = Timeout / 1000;
				tv.tv_usec = (Timeout % 1000) * 1000;

				FD_ZERO(&WriteFDSet);
				FD_ZERO(&ErrorFDSet);

				FD_SET(Socket.s, &WriteFDSet);
				FD_SET(Socket.s, &ErrorFDSet);

				CleanErrorCode(Socket);

				if (::select(1, nullptr, &WriteFDSet, &ErrorFDSet, &tv) <= 0) {
					Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
					R = -1;
				}

				else if (FD_ISSET(Socket.s, &ErrorFDSet)) {
					//s32 Err = 0, ErrLen = sizeof(s32);

					//Socket.err = ESocketError::Timedout;
					Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
				}

			}

			if (!RNonblk) {
				SetNonblock(Socket, false);
			}

			return R;
		}

		return -1;
	}

	ssize_t FSocketLayer::Connect(const FRawSocket & Socket, const FIPAddressV6 & Address, s32 Port, s32 Timeout)
	{
		if (Timeout < 0)
			return Connect(Socket, Address, Port);

		if (IsValid(Socket) && Socket.type == SOCK_STREAM) {
			bool RNonblk = IsNonblock(Socket);
			sockaddr_in6 SockAddr = { 0, };
			ssize_t R = -1, TrySelect = 1;

			SOCKET_IPv6ToSA6(SockAddr, Address);
			SockAddr.sin6_port = htons(u16(Port));

			if (!RNonblk) {
				SetNonblock(Socket, true);
			}

			CleanErrorCode(Socket);

			if ((R = ::connect(Socket.s, (sockaddr*)&SockAddr, sizeof(SockAddr))) != 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);

				if (Socket.err != ESocketError::InProgress &&
					Socket.err != ESocketError::WouldBlock &&
					Socket.err != ESocketError::Pending)
				{
					TrySelect = 0;
				}
			}

			if (TrySelect) {
				fd_set WriteFDSet, ErrorFDSet;
				timeval tv;

				tv.tv_sec = Timeout / 1000;
				tv.tv_usec = (Timeout % 1000) * 1000;

				FD_ZERO(&WriteFDSet);
				FD_ZERO(&ErrorFDSet);

				FD_SET(Socket.s, &WriteFDSet);
				FD_SET(Socket.s, &ErrorFDSet);

				CleanErrorCode(Socket);

				if (::select(1, nullptr, &WriteFDSet, &ErrorFDSet, &tv) <= 0) {
					Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
					R = -1;
				}

				else if (FD_ISSET(Socket.s, &ErrorFDSet)) {
					//s32 Err = 0, ErrLen = sizeof(s32);

					//Socket.err = ESocketError::Timedout;
					Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
				}

			}

			if (!RNonblk) {
				SetNonblock(Socket, false);
			}

			return R;
		}

		return -1;
	}

	ssize_t FSocketLayer::Shutdown(const FRawSocket& Socket)
	{
		if (IsValid(Socket)) {
			return ::shutdown(Socket.s, SHUT_RDWR);
		}

		return -1;
	}

	ssize_t FSocketLayer::ShutdownRead(const FRawSocket & Socket)
	{
		if (IsValid(Socket)) {
			return ::shutdown(Socket.s, SHUT_RD);
		}

		return -1;
	}

	ssize_t FSocketLayer::ShutdownWrite(const FRawSocket & Socket)
	{
		if (IsValid(Socket)) {
			return ::shutdown(Socket.s, SHUT_WR);
		}

		return -1;
	}

	ssize_t FSocketLayer::Recv(const FRawSocket& Socket, void* Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			ssize_t R = 0;

			CleanErrorCode(Socket);
			R = ::recv(Socket.s, (char*)Buffer, s32(Size), 0);

			if (R <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return R;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::Send(const FRawSocket& Socket, const void* Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			ssize_t S = 0;

			CleanErrorCode(Socket);
			S = ::send(Socket.s, (const char*)Buffer, s32(Size), 0);

			if (S <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return S;
		}

		Socket.err = ESocketError::BadSocket;
		return -1;
	}

	ssize_t FSocketLayer::RecvFrom(const FRawSocket& Socket, FIPAddress& From, u16& LocalPort, void* Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			sockaddr_in SockAddr = { 0, };
			socklen_t SockLen = sizeof(SockAddr);

			ssize_t S = 0;

			const u8* AddrBytes = (const u8*)(&SockAddr.sin_addr);

			CleanErrorCode(Socket);
			S = ::recvfrom(Socket.s, (char*)Buffer,
				s32(Size), 0, (sockaddr*)&SockAddr, &SockLen);

			if (S <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			LocalPort = ntohs(SockAddr.sin_port);

			From.SetByteAt(0, AddrBytes[0]);
			From.SetByteAt(1, AddrBytes[1]);
			From.SetByteAt(2, AddrBytes[2]);
			From.SetByteAt(3, AddrBytes[3]);

			return S;
		}

		return -1;
	}

	ssize_t FSocketLayer::SendTo(const FRawSocket& Socket, const FIPAddress& To, const u16& LocalPort, const void* Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			sockaddr_in SockAddr = { 0, };
			socklen_t SockLen = sizeof(SockAddr);

			u8* AddrBytes = (u8*)(&SockAddr.sin_addr);
			ssize_t S = -1;

			SockAddr.sin_port = htons(LocalPort);

			AddrBytes[0] = To.GetByteAt(0);
			AddrBytes[1] = To.GetByteAt(1);
			AddrBytes[2] = To.GetByteAt(2);
			AddrBytes[3] = To.GetByteAt(3);

			CleanErrorCode(Socket);
			S = ::sendto(Socket.s, (const char*)Buffer, s32(Size),
				0, (const sockaddr*)&SockAddr, SockLen);

			if (S <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return S;
		}

		return -1;
	}

	ssize_t FSocketLayer::RecvFrom(const FRawSocket & Socket, FIPAddressV6& From, u16& LocalPort, void * Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			sockaddr_in6 SockAddr = { 0, };
			socklen_t SockLen = sizeof(SockAddr);

			ssize_t S = 0;

			const u8* AddrBytes = (const u8*)(&SockAddr.sin6_addr);

			CleanErrorCode(Socket);
			S = ::recvfrom(Socket.s, (char*)Buffer,
				s32(Size), 0, (sockaddr*)&SockAddr, &SockLen);

			if (S <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			LocalPort = ntohs(SockAddr.sin6_port);

			for (s8 i = 0; i < 4; ++i) {
				From.SetByteAt(i * 4 + 0, AddrBytes[i * 4 + 0]);
				From.SetByteAt(i * 4 + 1, AddrBytes[i * 4 + 1]);
				From.SetByteAt(i * 4 + 2, AddrBytes[i * 4 + 2]);
				From.SetByteAt(i * 4 + 3, AddrBytes[i * 4 + 3]);
			}

			return S;
		}

		return -1;
	}

	ssize_t FSocketLayer::SendTo(const FRawSocket & Socket, const FIPAddressV6 & To, const u16 & LocalPort, const void * Buffer, size_t Size)
	{
		if (IsValid(Socket)) {
			sockaddr_in6 SockAddr = { 0, };
			socklen_t SockLen = sizeof(SockAddr);
			ssize_t S = -1;

			SOCKET_IPv6ToSA6(SockAddr, To);
			SockAddr.sin6_port = htons(LocalPort);

			CleanErrorCode(Socket);
			S = ::sendto(Socket.s, (const char*)Buffer, s32(Size),
				0, (const sockaddr*)&SockAddr, SockLen);

			if (S <= 0) {
				Socket.err = SOCKET_GetErrorCode(Socket.s, &Socket.err_raw);
			}

			return S;
		}

		return -1;
	}

	FASTINLINE u32 SOCKET_TranslatePollFlags(const ESocketChannel Channels) {
		u32 Translated = 0;

		if (Channels & ESOCK_INPUT) {
			Translated |= POLLIN;
		}

		if (Channels & ESOCK_OUTPUT) {
			Translated |= POLLOUT;
		}

		if (Channels & ESOCK_ERROR) {
			Translated |= POLLERR;
		}

		if (Channels & ESOCK_HANGUP) {
			Translated |= POLLHUP;
		}

		return Translated;
	}

	ssize_t FSocketLayer::Poll(FSocketPoll* Targets, size_t Count, const u32 Timeout)
	{
		static TMempool<pollfd, 8> Pool;

		if (Count) {
			pollfd* fds = Pool.Alloc(Count);
			ssize_t S = -1;

			for (size_t i = 0; i < Count; ++i) {
				fds[i].fd = Targets[i].Socket.s;
				fds[i].events = u16(SOCKET_TranslatePollFlags(Targets[i].Events));
				fds[i].revents = 0;

				Targets[i].Channels = ESOCK_NONE;
				CleanErrorCode(Targets[i].Socket);
			}

#if PLATFORM_WINDOWS
			S = WSAPoll(fds, u32(Count), Timeout);
#else
			S = poll(fds, u32(Count), Timeout);
#endif

			if (S < 0) {
				Pool.Free(fds); // ERROR.
				return -1;
			}

			else if (S) {
				for (size_t i = 0; i < Count; ++i) {
					u32 flags = fds[i].revents;

					if (flags & POLLIN) {
						Targets[i].Channels = (ESocketChannel)(
							Targets[i].Channels | ESOCK_INPUT);
					}

					if (flags & POLLOUT) {
						Targets[i].Channels = (ESocketChannel)(
							Targets[i].Channels | ESOCK_OUTPUT);
					}

					if (flags & POLLERR) {
						Targets[i].Channels = (ESocketChannel)(
							Targets[i].Channels | ESOCK_ERROR);
					}

					if (flags & POLLHUP) {
						Targets[i].Channels = (ESocketChannel)(
							Targets[i].Channels | ESOCK_HANGUP);
					}

					Targets[i].Socket.err = SOCKET_GetErrorCode(
						Targets[i].Socket.s, &Targets[i].Socket.err_raw);
				}
			}
			else {
				for (size_t i = 0; i < Count; ++i) {
					Targets[i].Socket.err = SOCKET_GetErrorCode(
						Targets[i].Socket.s, &Targets[i].Socket.err_raw);
				}
			}

			Pool.Free(fds);
			return S;
		}

		return 0;
	}

}