#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/network/IPAddress.hpp>
#include <boxpp/base/network/SocketLayer.hpp>

namespace boxpp
{
	class BOXPP FSocket
	{
	protected:
		FSocket();
		FSocket(EProtocolType Protocol, ESocketType SocketType);

		FSocket(FSocket&& Other);
		FSocket(const FSocket&) = delete;

	public:
		~FSocket();

	public:
		FASTINLINE operator bool() const { return FSocketLayer::IsValid(Socket); }
		FASTINLINE bool operator !() const { return !FSocketLayer::IsValid(Socket); }

	public:
		FSocket& operator =(FSocket&& Other);
		FSocket& operator =(const FSocket&) = delete;

	public:
		FASTINLINE bool HasError() const { return FSocketLayer::IsSocketCorrupted(Socket); }

		FASTINLINE ESocketError GetError() const { return Socket.err; }
		FASTINLINE bool ShouldAgain() const { return FSocketLayer::ShouldAgain(Socket); }

		FASTINLINE bool IsNonblock() const { return FSocketLayer::IsNonblock(Socket); }
		FASTINLINE bool IsNoDelay() const { return FSocketLayer::IsNoDelay(Socket); }

	public:
		FASTINLINE bool SetNonblock(bool v) { return !FSocketLayer::SetNonblock(Socket, v); }
		FASTINLINE bool SetNoDelay(bool v) { return !FSocketLayer::SetNoDelay(Socket, v); }

	public:
		FASTINLINE bool Bind(const FIPAddress& Address, s32 Port) { return !FSocketLayer::Bind(Socket, Address, Port); }
		FASTINLINE bool Bind(const FIPAddressV6& Address, s32 Port) { return !FSocketLayer::Bind(Socket, Address, Port); }
		FASTINLINE bool Listen(s32 Backlog) { return !FSocketLayer::Listen(Socket, Backlog); }

		FASTINLINE bool Pending() {
			return Poll(ESocketChannel::ESOCK_INPUT, 0) == ESOCK_INPUT;
		}

		bool Accept(FSocket& Newbie);

	public:
		FASTINLINE bool Connect(const FIPAddress& Address, s32 Port, s32 Timeout = -1) {
			return FSocketLayer::Connect(Socket, Address, Port, Timeout);
		}

		FASTINLINE bool Connect(const FIPAddressV6& Address, s32 Port, s32 Timeout = -1) {
			return FSocketLayer::Connect(Socket, Address, Port, Timeout);
		}

		FASTINLINE bool Shutdown(bool Rd, bool Wr) {
			if (Rd && Wr) {
				return !FSocketLayer::Shutdown(Socket);
			}

			else if (Rd) {
				return !FSocketLayer::ShutdownRead(Socket);
			}

			else if (Wr) {
				return !FSocketLayer::ShutdownWrite(Socket);
			}

			return false;
		}

		FASTINLINE ssize_t Receive(void* Buffer, size_t Size) {
			return FSocketLayer::Recv(Socket, Buffer, Size);
		}

		FASTINLINE ssize_t Send(const void* Buffer, size_t Size) {
			return FSocketLayer::Send(Socket, Buffer, Size);
		}

		FASTINLINE ssize_t ReceiveFrom(FIPAddress& From, u16& LocalPort, void* Buffer, size_t Size) {
			return FSocketLayer::RecvFrom(Socket, From, LocalPort, Buffer, Size);
		}

		FASTINLINE ssize_t SendTo(const FIPAddress& To, const u16& LocalPort, const void* Buffer, size_t Size) {
			return FSocketLayer::SendTo(Socket, To, LocalPort, Buffer, Size);
		}

		FASTINLINE ssize_t ReceiveFrom(FIPAddressV6& From, u16& LocalPort, void* Buffer, size_t Size) {
			return FSocketLayer::RecvFrom(Socket, From, LocalPort, Buffer, Size);
		}

		FASTINLINE ssize_t SendTo(const FIPAddressV6& To, const u16& LocalPort, const void* Buffer, size_t Size) {
			return FSocketLayer::SendTo(Socket, To, LocalPort, Buffer, Size);
		}

	public:
		ESocketChannel Poll(ESocketChannel Channels, const u32 Timeout = 0);

	public:
		static ssize_t Poll(TArray<FSocket*>& Sockets, ESocketChannel Channels, const u32 Timeout = 0);

	private:
		FRawSocket Socket;

	public:
		bool Close();
	};

}