#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

/* placement new from std. */
#include <new>

#include <boxpp/base/tpls/strings/String.hpp>

namespace boxpp
{
	class BOXPP FIPAddress
	{
	public:
		struct __AnyType { s32 _; };
		struct __LoopbackType { s16 _; };
		struct __MaskType { s8 _; };

	public:
		static constexpr __AnyType Any = { 0 };
		static constexpr __LoopbackType Loopback = { 1 };
		static constexpr __MaskType Mask = { 2 };

	public:
		/* Any address: 0.0.0.0. */
		FASTINLINE FIPAddress(__AnyType = Any) 
			: AddressWidth(0) { AddressDword = 0; }

		/* Loopback address: 127.0.0.1. */
		FASTINLINE FIPAddress(__LoopbackType) 
			: AddressWidth(32)
		{
			AddressDword = 0;

			AddressBytes[0] = 127;
			AddressBytes[3] = 1;
		}

		/*
			Masking address.
			Usage: FIPAddress Something(FIPAddress::Mask, "127.0.0.0", 24);
			 --> 127.0.0.1 ~ 127.0.0.255
		*/
		FIPAddress(__MaskType, const ansi_t* Address, s8 Width);
		FIPAddress(__MaskType, const wide_t* Address, s8 Width);

		/* Stringified address to parsed IPAddress. */
		FIPAddress(const ansi_t* Address);
		FIPAddress(const wide_t* Address);

		FIPAddress(const FIPAddress&) = default;
		FIPAddress(FIPAddress&&) = default;

	public:
		FASTINLINE FIPAddress& operator =(__AnyType) { AddressDword = AddressWidth = 0; return *this; }
		FASTINLINE FIPAddress& operator =(__LoopbackType) {
			AddressDword = 0;
			AddressWidth = 32;

			AddressBytes[0] = 127;
			AddressBytes[3] = 1;
			return *this;
		}

		FIPAddress& operator =(const FIPAddress&) = default;
		FIPAddress& operator =(FIPAddress&&) = default;

	public:
		FASTINLINE operator bool() const { return AddressWidth >= 0; }
		FASTINLINE bool operator !() const { return AddressWidth < 0; }

	public:
		FASTINLINE bool IsAnycast() const { return (AddressDword == 0 && AddressWidth > 0) || AddressWidth == 0; }
		FASTINLINE bool IsLoopback() const { return AddressBytes[0] == 127 && AddressWidth >= 8; }
		FASTINLINE bool IsMasking() const { return AddressWidth >= 0 && AddressWidth < 32; }
		FASTINLINE u8 GetWidth() const { return AddressWidth; }

	public:
		bool ToString(FWideString& OutString);
		bool ToString(FAnsiString& OutString);

	public:
		FASTINLINE u32 GetDword() const { return AddressDword; }
		FASTINLINE void SetDword(u32 Value) { AddressDword = Value; }

		FASTINLINE u8 GetByteAt(s8 Offset) const {
			return (Offset >= 0 && Offset < 4) ? AddressBytes[Offset] : 0; 
		};

		FASTINLINE void SetByteAt(s8 Offset, u8 Value) {
			if (Offset >= 0 && Offset < 4)
				AddressBytes[Offset] = Value;
		}

		FASTINLINE void SetWidth(u8 Width) {
			AddressWidth = Width < 32 ? Width : 32;
		}

	private:
		union {
			u32 AddressDword;
			u8 AddressBytes[4];
		};

		s8 AddressWidth;
	};
}