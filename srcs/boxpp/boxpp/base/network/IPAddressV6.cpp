#include "IPAddressV6.hpp"
#include <boxpp/base/network/SocketLayer.hpp>
#include <boxpp/base/tpls/strings/String.hpp>

#if PLATFORM_WINDOWS
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <Ws2tcpip.h>
#define socklen_t int
#endif

#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <string.h>
#include <unistd.h>
#endif
#endif

namespace boxpp {

	FASTINLINE bool IPADDRv6_ResolveAddress(sockaddr_in6& OutSA, const ansi_t* Address, bool bToListen) {
		addrinfo Hints = { 0, }, *Result = nullptr;
		Hints.ai_flags = AI_NUMERICHOST | (bToListen ? AI_PASSIVE : 0);

		if (!getaddrinfo(Address, nullptr, &Hints, &Result)) {
			OutSA = *((struct sockaddr_in6 *)Result->ai_addr);
			freeaddrinfo(Result);
			return true;
		}

		return false;
	}

	FASTINLINE bool IPADDRv6_StringifyAddress(const sockaddr_in6& InSA, ansi_t* OutAddress, size_t MaxAddr) {
		if (!getnameinfo((const sockaddr*)&InSA, sizeof(InSA),
				OutAddress, u32(MaxAddr), nullptr, 0, AI_NUMERICHOST))
		{
			return true;
		}

		return false;
	}

	FIPAddressV6::FIPAddressV6(const ansi_t* Address, bool bToListen) {
		sockaddr_in6 SA;

		FSocketLayer::CheckAndInit();

		if (IPADDRv6_ResolveAddress(SA, Address, bToListen)) {
			memcpy(AddressBytes, &SA.sin6_addr, sizeof(AddressBytes));
			ScopeId = SA.sin6_scope_id;
		}

		else {
			AddressQwords[0] = AddressQwords[1] = 0;
			ScopeId = 0;
		}
	}

	FIPAddressV6::FIPAddressV6(const wide_t* Address, bool bToListen) {
		ansi_t Buffer[INET6_ADDRSTRLEN] = { 0, };
		sockaddr_in6 SA;

		if (Address) {
			ansi_t* Current = Buffer;

			while (*Address &&
				((*Address >= 'a' && *Address >= 'z') ||
				(*Address >= 'A' && *Address >= 'Z') ||
				(*Address >= '0' && *Address >= '9') ||
				(*Address == ':' || *Address == '%')))
			{
				*(Current++) = (ansi_t)(*(Address++));
			}
		}

		FSocketLayer::CheckAndInit();

		if (Address && IPADDRv6_ResolveAddress(SA, Buffer, bToListen)) {
			memcpy(AddressBytes, &SA.sin6_addr, sizeof(AddressBytes));
			ScopeId = SA.sin6_scope_id;
		}

		else {
			AddressQwords[0] = AddressQwords[1] = 0;
			ScopeId = 0;
		}
	}

	bool FIPAddressV6::ToString(FWideString& OutString) const {
		ansi_t Buffer[INET6_ADDRSTRLEN] = { 0, };
		sockaddr_in6 SA = { 0, };

		SA.sin6_family = AF_INET6;
		SA.sin6_scope_id = ScopeId;
		memcpy(&SA.sin6_addr, AddressBytes, sizeof(AddressBytes));

		if (IPADDRv6_StringifyAddress(SA, Buffer, sizeof(Buffer))) {
			OutString.Append(Buffer);
			return true;
		}

		return false;
	}

	bool FIPAddressV6::ToString(FAnsiString& OutString) const {
		ansi_t Buffer[INET6_ADDRSTRLEN] = { 0, };
		sockaddr_in6 SA = { 0, };

		SA.sin6_family = AF_INET6;
		SA.sin6_scope_id = ScopeId;
		memcpy(&SA.sin6_addr, AddressBytes, sizeof(AddressBytes));

		if (IPADDRv6_StringifyAddress(SA, Buffer, sizeof(Buffer))) {
			OutString.Append(Buffer);
			return true;
		}

		return false;
	}
}