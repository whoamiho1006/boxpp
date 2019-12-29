#include "TcpClient.hpp"

namespace boxpp {

	bool FTcpClient::Connect(const FIPEndpoint& Endpoint, s32 Timeout)
	{
		if (!Socket && !bConnected) {
			Socket = FSocket(EProtocolType::Inet, ESocketType::Tcp);
			
			if (Socket.Connect(Endpoint, Timeout)) {
				bConnected = true;
				return true;
			}

			Socket.Close();
		}

		return false;
	}

	bool FTcpClient::Connect(const FIPEndpointV6 & Endpoint, s32 Timeout)
	{
		if (!Socket && !bConnected) {
			Socket = FSocket(EProtocolType::Inet6, ESocketType::Tcp);

			if (Socket.Connect(Endpoint, Timeout)) {
				bConnected = true;
				return true;
			}

			Socket.Close();
		}

		return false;
	}

	bool FTcpClient::Disconnect()
	{
		if (Socket && bConnected) {
			Socket.Shutdown();
			bConnected = false;
			return true;
		}

		return false;
	}

	bool FTcpClient::Close()
	{
		if (Socket) {
			Socket.Close();
			bConnected = false;
			return true;
		}

		return false;
	}

	ssize_t FTcpClient::Receive(void* Buffer, size_t Size)
	{
		if (Socket && bConnected) {
			ssize_t R = Socket.Receive(Buffer, Size);

			if (R <= 0 && Socket.HasError()) {
				bConnected = false;
			}

			return R;
		}

		return -1;
	}

	ssize_t FTcpClient::Send(const void* Buffer, size_t Size)
	{
		if (Socket && bConnected) {
			ssize_t R = Socket.Send(Buffer, Size);

			if (R <= 0 && Socket.HasError()) {
				bConnected = false;
			}

			return R;
		}

		return -1;
	}

}