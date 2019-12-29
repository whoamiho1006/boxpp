#include "TcpListener.hpp"
#include "TcpClient.hpp"

namespace boxpp 
{
	FTcpListener::FTcpListener(EProtocolType Protocol, s32 Port)
		: Backlog(10), bListening(false), bCritical(false), 
		  Socket(Protocol, ESocketType::Tcp)
	{
		if (Protocol == EProtocolType::Inet) {
			Endpoint = FIPEndpoint(FIPAddress::Any, Port);
		}

		else {
			EndpointV6 = FIPEndpointV6(FIPAddressV6::Any, Port);
		}
	}

	FTcpListener::FTcpListener(const FIPEndpoint& Endpoint)
		: Backlog(10), bListening(false), bCritical(false), 
		  Socket(EProtocolType::Inet, ESocketType::Tcp), Endpoint(Endpoint)
	{
	}

	FTcpListener::FTcpListener(const FIPEndpointV6& Endpoint)
		: Backlog(10), bListening(false), bCritical(false), 
		  Socket(EProtocolType::Inet6, ESocketType::Tcp), EndpointV6(Endpoint)
	{
	}
	
	bool FTcpListener::Start()
	{
		if (!bListening) {
			bool bBound = false;

			if (Endpoint) {
				if (bCritical) {
					Socket = FSocket(EProtocolType::Inet, ESocketType::Tcp);
					bCritical = false;
				}

				Socket.SetReusable();
				bBound = Socket.Bind(Endpoint);
			}
			else if (EndpointV6) {
				if (bCritical) {
					Socket = FSocket(EProtocolType::Inet6, ESocketType::Tcp);
					bCritical = false;
				}

				Socket.SetReusable();
				bBound = Socket.Bind(EndpointV6);
			}

			BOX_ENSURE(bBound);
			if (!bBound) {
				Socket.Close();
				bCritical = true;
				return false;
			}

			bBound = Socket.Listen(Backlog);
			BOX_ENSURE(bBound);

			if (!bBound) {
				Socket.Close();
				bCritical = true;
				return false;
			}

			bListening = true;
			OnStartup(Socket);
			return true;
		}

		return false;
	}

	bool FTcpListener::Stop()
	{
		if (bListening) {
			OnShutdown(Socket);

			Socket.Close();
			bCritical = true;
			return true;
		}

		return false;
	}

	bool FTcpListener::IsPending() const
	{
		if (bListening) {
			return Socket.Pending();
		}

		return false;
	}

	bool FTcpListener::Accept(FTcpClient& OutClient)
	{
		if (bListening) 
		{
			FSocket OutSocket;

			if (Socket.Accept(OutSocket) &&
				OnAccept(OutSocket))
			{
				OutClient = Forward<FSocket>(OutSocket);
				return true;
			}

			else if (OutSocket) {
				OutSocket.Close();
			}
		}

		return bListening;
	}


}