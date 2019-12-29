#pragma once
#include <boxpp/base/network/Socket.hpp>

namespace boxpp
{
	class FTcpListener;
	class BOXPP FTcpClient
	{
		friend class FTcpListener;

	public:
		FTcpClient() : bConnected(false) { }
		FTcpClient(const FTcpClient&) = delete;

		FASTINLINE FTcpClient(FTcpClient&& Other) 
			: bConnected(false)
		{
			Swap(Socket, Other.Socket);
		}
		
	private:
		mutable FSocket Socket;
		mutable bool bConnected;

	public:
		FASTINLINE FSocket* GetSocket() const { return &Socket; }
		FASTINLINE bool IsConnected() const { return bConnected; }

	public:
		FTcpClient& operator =(const FTcpClient& Other) = delete;
		FASTINLINE FTcpClient& operator =(FTcpClient&& Other) {
			if (this != &Other) {
				Swap(Socket, Other.Socket);
				Swap(bConnected, Other.bConnected);
			}

			return *this;
		}

	protected:
		/* Tcp Listener only assignment operator. */
		FASTINLINE FTcpClient& operator =(FSocket&& Newbie) {
			Swap(Socket, Newbie);
			bConnected = true;
			return *this;
		}

	public:
		bool Connect(const FIPEndpoint& Endpoint, s32 Timeout = -1);
		bool Connect(const FIPEndpointV6& Endpoint, s32 Timeout = -1);

	public:
		bool Disconnect();
		bool Close();

	public:
		ssize_t Receive(void* Buffer, size_t Size);
		ssize_t Send(const void* Buffer, size_t Size);
	};
}