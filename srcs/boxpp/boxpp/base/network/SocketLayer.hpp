#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/network/IPAddress.hpp>

namespace boxpp
{
	enum class ESocketType
	{
		/* Tcp. */
		Tcp, 

		/* Udp. */
		Udp
	};

	enum class EProtocolType
	{
		/* Internet Protocol v4 */
		Inet, 

		/* Internet Protocol v6 */
		Inet6
	};

	enum class ESocketError
	{
		Success = 0,

		/* Non-blocking response codes. */
		Pending,

#if PLATFORM_WINDOWS
		WouldBlock,
#else
		WouldBlock = Pending,
#endif

		InProgress,
#if PLATFORM_WINDOWS
		Incompleted,
#else
		Incompleted = InProgress,
#endif

		/* Already doing something */
		Already,
		Cancelled,

		/* Interrupted. */
		Interrupted,

		/* Not wrapped error. */
		NotWrapped,

		/* Handle, OS related errors. */
		BadSocket,
		NoResources, // = TooManyRefs, TooManySockets, TooManyProcesses

		/* Creation error codes. */
		BadAddress,
		NoDestination,
		AddressInUse,
		AddressNotAvailable,
		InvalidProtocol,
		InvalidProtocolOptions,
		InvalidArgument,

		/* Requests about unsupported features... */
		NotSupportedProtocol,
		NotSupportedSocket,
		NotSupportedOperation,
		NotSupportedFamily,

		/* Host system related error codes. */
		HostDown,
		HostUnreachable,
		HostNotFound, // => Windows only.

		/* Network related error codes. */
		NetworkDown,
		NetworkUnreachable,
		
		/* Error codes before connection. */
		Aborted,
		Refused,
		NotConnected,

		/* Error codes after connection. */
		NoBuffer,
		Timedout,
		Shutdown,
		ResetByPeer,
		ResetByNetwork,
		ConnectedAlready,
	};

	/* Raw socket container. */
	struct FRawSocket { 
		s32 s, type, family;
		mutable s32 err_raw;
		mutable ESocketError err;
	};

	/*
		Socket Layer.
		Use FTcpSocket, FUdpSocket, FTcpServerSocket, FUdpServerSocket instead of this functions.
	*/
	class BOXPP FSocketLayer
	{
	public:
		static FRawSocket Create(EProtocolType ProtocolType, ESocketType SocketType);
		static void Close(FRawSocket& Socket);

	public:
		FASTINLINE static bool IsValid(const FRawSocket& Socket) { return Socket.s >= 0; }
		FASTINLINE static ESocketError GetErrorCode(const FRawSocket& Socket) { return Socket.err; }

		/* Determines the socket is corrupted and dead, so unusable or not. */
		FASTINLINE static bool IsSocketCorrupted(const FRawSocket& Socket) {
			switch (Socket.err) {
			case ESocketError::Success:
			case ESocketError::WouldBlock:
#if PLATFORM_WINDOWS || (PLATFORM_POSIX && EAGAIN != EWOULDBLOCK)
			case ESocketError::Pending:
#endif
#if PLATFORM_WINDOWS
			case ESocketError::Incompleted:
#endif
			case ESocketError::InProgress:
			case ESocketError::Interrupted:
				return false;

			default:
				break;
			}

			return true;
		}

		/* Determines the socket has in-completed task and some action should be tried again, or not. */
		FASTINLINE static bool ShouldAgain(const FRawSocket& Socket) {
			switch (Socket.err) {
			case ESocketError::WouldBlock:
#if PLATFORM_WINDOWS || (PLATFORM_POSIX && EAGAIN != EWOULDBLOCK)
			case ESocketError::Pending:
#endif
#if PLATFORM_WINDOWS
			case ESocketError::Incompleted:
#endif
			case ESocketError::InProgress:
			case ESocketError::Interrupted:
				return true;

			default:
				break;
			}

			return false;
		}

	public:
		static bool IsNonblock(const FRawSocket& Socket);
		static ssize_t SetNonblock(const FRawSocket& Socket, bool Value);

	public:
		static bool IsNoDelay(const FRawSocket& Socket);
		static ssize_t SetNoDelay(const FRawSocket& Socket, bool Value);

		static s64 GetRecvTimeout(const FRawSocket& Socket);
		static s64 GetSendTimeout(const FRawSocket& Socket);

		static ssize_t SetRecvTimeout(const FRawSocket& Socket, s32 Timeout); /* Time-unit: ms */
		static ssize_t SetSendTimeout(const FRawSocket& Socket, s32 Timeout); /* Time-unit: ms */

	public:
		static ssize_t Bind(const FRawSocket& Socket, const FIPAddress& Address, s32 Port);
		static ssize_t Listen(const FRawSocket& Socket, s32 Backlog);
		static ssize_t Accept(const FRawSocket& Socket, FRawSocket& Newbie);

	public:
		static ssize_t Connect(const FRawSocket& Socket, const FIPAddress& Address, s32 Port);
		static ssize_t Connect(const FRawSocket& Socket, const FIPAddress& Address, s32 Port, s32 Timeout);

	public:
		static ssize_t Shutdown(const FRawSocket& Socket);
		static ssize_t ShutdownRead(const FRawSocket& Socket);
		static ssize_t ShutdownWrite(const FRawSocket& Socket);

	public:
		static ssize_t Recv(const FRawSocket& Socket, void* Buffer, size_t Size);
		static ssize_t Send(const FRawSocket& Socket, const void* Buffer, size_t Size);
	};
}