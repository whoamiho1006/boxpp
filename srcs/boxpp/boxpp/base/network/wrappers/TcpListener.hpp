#pragma once
#include <boxpp/base/network/Socket.hpp>

namespace boxpp
{
	class FTcpClient;
	class BOXPP FTcpListener
	{
	public:
		FTcpListener(EProtocolType Protocol, s32 Port);
		FTcpListener(const FIPEndpoint& Endpoint);
		FTcpListener(const FIPEndpointV6& Endpoint);
		virtual ~FTcpListener() { Stop(); }

	private:
		u32 Backlog;
		bool bListening, bCritical;
		mutable FSocket Socket;

		FIPEndpoint Endpoint;
		FIPEndpointV6 EndpointV6;

	public:
		FASTINLINE bool IsListening() const { return bListening; }
		FASTINLINE FSocket* GetSocket() const { return &Socket; }

		FASTINLINE u32 GetBacklog() const { return Backlog; }
		FASTINLINE void SetBacklog(u32 V) {
			if (!bListening) {
				Backlog = V;
			}
		}

	public:
		bool Start();
		bool Stop();

	public:
		bool IsPending() const;
		bool Accept(FTcpClient& OutClient);

	protected:
		virtual void OnStartup(FSocket& ServerSocket) { }
		virtual void OnShutdown(FSocket& ServerSocket) { }

	protected:
		virtual bool OnAccept(FSocket& Newbie) { return true; }

	};

}