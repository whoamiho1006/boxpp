#include "IPAddress.hpp"

#include <boxpp/base/systems/Debugger.hpp>
#include <boxpp/base/opacities/vsprintf.hpp>

namespace boxpp {
	FIPAddress::FIPAddress(__MaskType k, const ansi_t* Address, s8 Width)
		: FIPAddress(Address)
	{
		BOX_ASSERT(k._ == 2, "Invalid compilation: FIPAddress::Mask");
		AddressWidth = Width;
	}

	FIPAddress::FIPAddress(__MaskType k, const wide_t * Address, s8 Width)
		: FIPAddress(Address)
	{
		BOX_ASSERT(k._ == 2, "Invalid compilation: FIPAddress::Mask");
		AddressWidth = Width;
	}

	template<typename CharType>
	FASTINLINE void FIPAddress_ParseString(
		const CharType* String, u32& OutDword, s8& OutWidth)
	{
		s32 Digit = 0, Error = 0;

		union {
			u32 AddressDword;
			u8 AddressBytes[4];
		};

		while (*String) {
			u32 Val = u32(TNativeString<CharType>::Atoul(String));
			if (Val > 0xff) {
				Error = 1;
				break;
			}

			AddressBytes[Digit++] = u8(Val);


			while (*String && *String != TNativeString<CharType>::Dot) ++String;
			if (*String != TNativeString<CharType>::Dot) {
				if (Digit >= 3)
					break;

				Error = 1;
				break;
			}
		}

		if (!Error) {
			OutDword = AddressDword;
			OutWidth = 32;
		}
	}

	FIPAddress::FIPAddress(const ansi_t* Address)
		: AddressDword(0xFFFFFFFFu), AddressWidth(-1)
	{
		FIPAddress_ParseString(Address, AddressDword, AddressWidth);
	}

	FIPAddress::FIPAddress(const wide_t* Address)
		: AddressDword(0xFFFFFFFFu), AddressWidth(-1)
	{
		FIPAddress_ParseString(Address, AddressDword, AddressWidth);
	}

	bool FIPAddress::ToString(FWideString& OutString) const
	{
		if (AddressWidth >= 0) {
			wide_t Buffer[24] = { 0, };

			TVsprintf<wide_t>::Sprintf(
				Buffer, L"%d.%d.%d.%d",
				AddressBytes[0], AddressBytes[1],
				AddressBytes[2], AddressBytes[3]);

			OutString.Append(Buffer);
			return true;
		}

		return false;
	}

	bool FIPAddress::ToString(FAnsiString& OutString) const
	{
		if (AddressWidth >= 0) {
			ansi_t Buffer[24] = { 0, };

			TVsprintf<ansi_t>::Sprintf(
				Buffer, "%d.%d.%d.%d",
				AddressBytes[0], AddressBytes[1],
				AddressBytes[2], AddressBytes[3]);

			OutString.Append(Buffer);
			return true;
		}

		return false;
	}

}