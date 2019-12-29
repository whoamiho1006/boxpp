#pragma once
#include <boxpp/base/network/IPAddress.hpp>
#include <boxpp/base/network/IPAddressV6.hpp>

namespace boxpp
{
	template<typename AddressType = FIPAddress>
	class TIPEndpoint
	{
		static_assert(
			IsSameType<AddressType, FIPAddress> || IsSameType<AddressType, FIPAddressV6>,
			"TIPEndpoint<...> supports only FIPAddress and FIPAddressV6!");

	public:
		TIPEndpoint()
			: Port(0)
		{
		}

		TIPEndpoint(const ansi_t* InString, bool bToListen = false)
			: Port(0)
		{
			ParseFromString<ansi_t>(InString, bToListen);
		}

		TIPEndpoint(const wide_t* InString, bool bToListen = false)
			: Port(0)
		{
			ParseFromString<wide_t>(InString, bToListen);
		}

		TIPEndpoint(const AddressType& Address, s32 Port)
			: Address(Address), Port(Port)
		{
		}

		TIPEndpoint(AddressType&& Address, s32 Port)
			: Address(Forward<AddressType>(Address)), Port(Port)
		{
		}

	private:
		template<typename CharType>
		FASTINLINE bool ParseFromString(const CharType* InString, bool bToListen = false) {
			using NString = TNativeString<CharType>;

			if (IsSameType<AddressType, FIPAddressV6>) {
				static const CharType IPv6_Begin[2] = { '[', 0 };
				static const CharType IPv6_Mid[3] = { ']', ':', 0 };

				if (!NString::Strncmp(InString, IPv6_Begin)) {
					ansi_t Buffer[65] = { 0, };
					ssize_t S = 0;

					// [A:D:D:R]:PORT
					while (S < 64 &&
						InString[S] &&
						InString[S] != ']')
					{
						Buffer[S] = InString[S];
					}

					Address = AddressType(Buffer, bToListen);
					if (NString::Strncmp(InString + S, IPv6_Mid)) {
						Port = NString::Atol(InString + S + 2);
					}
				}

				else {
					Address = AddressType(InString, bToListen);
				}
			}

			else if (IsSameType<AddressType, FIPAddress>) {
				ansi_t Buffer[16] = { 0, };
				ssize_t S = 0;

				// 000.000.000.000:PORT
				while (S < 15 &&
					InString[S] &&
					InString[S] != ':')
				{
					Buffer[S] = InString[S];
				}

				Address = AddressType(Buffer, bToListen);
				if (InString[S] == ':') {
					Port = NString::Atol(InString + S + 1);
				}
			}
		}

	private:
		AddressType Address;
		s32 Port;

	public:
		FASTINLINE operator bool() const { return Address && Port; }
		FASTINLINE bool operator !() const { return !Address || !Port; }

	public:
		FASTINLINE const AddressType& GetAddress() const { return Address; }
		FASTINLINE s32 GetPort() const { return Port; }

		FASTINLINE void SetAddress(const AddressType& Addr) { Address = Addr; }
		FASTINLINE void SetAddress(AddressType&& Addr) { Address = Forward<AddressType>(Addr); }
		FASTINLINE void SetPort(s32 Port) { this->Port = Port; }

	public:
		FASTINLINE bool operator ==(const TIPEndpoint<AddressType>& Other) const {
			return Address == Other.Address && Port == Other.Port;
		}

		FASTINLINE bool operator !=(const TIPEndpoint<AddressType>& Other) const {
			return Address != Other.Address || Port != Other.Port;
		}

	public:
		template<typename CharType>
		FASTINLINE bool ToString(TString<CharType>& OutString) const {
			if (IsSameType<AddressType, FIPAddressV6>) {
				static const CharType IPv6_Mid[3] = { ']', ':', 0 };

				size_t Offset = OutString.GetSize();

				if (Address.ToString(OutString)) {
					if (Port > 0) {
						OutString.Insert(Offset, '[');
						OutString.Append(IPv6_Mid);
						OutString.Append(TString<CharType>::FromInt(Port));
					}

					return true;
				}
			}

			else if (IsSameType<AddressType, FIPAddress>) {
				if (Address.ToString(OutString)) {
					if (Port > 0) {
						OutString.Append(':');
						OutString.Append(TString<CharType>::FromInt(Port));
					}

					return true;
				}
			}

			return false;
		}

		FASTINLINE FString ToString() const {
			FString String;

			ToString(String);

			return String;
		}

	};

	typedef TIPEndpoint<FIPAddress> FIPEndpoint;
	typedef TIPEndpoint<FIPAddressV6> FIPEndpointV6;
}