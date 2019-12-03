#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/network/IPAddress.hpp>

namespace boxpp
{
	class BOXPP FSocket
	{
	public:
		struct __TcpType { s32 _1; };
		struct __UdpType { s16 _2; };

	public:
		constexpr static __TcpType Tcp = { 1 };
		constexpr static __UdpType Udp = { 2 };

	protected:
		FSocket();

	public:
		FSocket(__TcpType);
		FSocket(__UdpType);

		FSocket(FSocket&& Other);
		FSocket(const FSocket&) = delete;

	public:
		~FSocket();

	public:
		FSocket& operator =(FSocket&& Other);
		FSocket& operator =(const FSocket&) = delete;

	private:
		s32 Socket;
	};

}