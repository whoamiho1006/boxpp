#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

/* placement new from std. */
#include <new>

#include <boxpp/base/tpls/strings/String.hpp>

namespace boxpp
{
	class BOXPP FIPAddressV6
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
		FASTINLINE FIPAddressV6(__AnyType = Any) {
			AddressQwords[0] = AddressQwords[1] = 0;
			ScopeId = 0;
		}

		/* Loopback address: ::1. */
		FASTINLINE FIPAddressV6(__LoopbackType) {
			AddressQwords[0] = AddressQwords[1] = 0;
			AddressBytes[15] = 1; ScopeId = 0;
		}

		/* Stringified address to parsed IPAddress. */
		FIPAddressV6(const ansi_t* Address, bool bToListen = false);
		FIPAddressV6(const wide_t* Address, bool bToListen = false);

		FIPAddressV6(const FIPAddressV6&) = default;
		FIPAddressV6(FIPAddressV6&&) = default;

	public:
		FASTINLINE FIPAddressV6& operator =(__AnyType) { 
			AddressQwords[0] = AddressQwords[1] = 0;
			return *this; 
		}
		FASTINLINE FIPAddressV6& operator =(__LoopbackType) {
			AddressQwords[0] = AddressQwords[1] = 0;
			AddressBytes[15] = 1;
			return *this;
		}

		FIPAddressV6& operator =(const FIPAddressV6&) = default;
		FIPAddressV6& operator =(FIPAddressV6&&) = default;

	public:
		FASTINLINE operator bool() const { return AddressQwords[0] || AddressQwords[1]; }
		FASTINLINE bool operator !() const { return !AddressQwords[0] && !AddressQwords[1]; }

	public:
		FASTINLINE bool operator ==(const FIPAddressV6& Other) const {
			return	AddressQwords[0] == Other.AddressQwords[0] &&
					AddressQwords[1] == Other.AddressQwords[1] &&
					ScopeId == Other.ScopeId;
		}

		FASTINLINE bool operator !=(const FIPAddressV6& Other) const {
			return	AddressQwords[0] != Other.AddressQwords[0] ||
					AddressQwords[1] != Other.AddressQwords[1] ||
					ScopeId != Other.ScopeId;
		}

	public:
		FASTINLINE bool IsAnycast() const { return !AddressQwords[0] && !AddressQwords[1]; }
		FASTINLINE bool IsLoopback() const { 
			return AddressBytes[15] == 1 && (
				!AddressBytes[0] && !AddressBytes[1] && !AddressBytes[2] && !AddressBytes[3] &&
				!AddressBytes[4] && !AddressBytes[5] && !AddressBytes[6] && !AddressBytes[7] &&
				!AddressBytes[8] && !AddressBytes[9] && !AddressBytes[10] && !AddressBytes[11] &&
				!AddressBytes[12] && !AddressBytes[13] && !AddressBytes[14]);
		}

	public:
		bool ToString(FWideString& OutString) const;
		bool ToString(FAnsiString& OutString) const;

		FASTINLINE FString ToString() const {
			FString String;

			ToString(String);

			return String;
		}

	public:
		FASTINLINE u64 GetQword(u32 Index) const { return AddressQwords[Index % 2]; }
		FASTINLINE void SetQword(u32 Index, u64 Value) { AddressQwords[Index % 2] = Value; }

		FASTINLINE u32 GetScopeId() const { return ScopeId; }
		FASTINLINE void SetScopeId(u32 Value) { ScopeId = Value; }

		FASTINLINE u8 GetByteAt(s8 Offset) const {
			return (Offset >= 0 && Offset < 16) ? AddressBytes[Offset] : 0;
		};

		FASTINLINE void SetByteAt(s8 Offset, u8 Value) {
			if (Offset >= 0 && Offset < 16)
				AddressBytes[Offset] = Value;
		}

	private:
		union {
			u64 AddressQwords[2];
			u8 AddressBytes[16];
		};

		u32 ScopeId;
	};
}