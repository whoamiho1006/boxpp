#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/IsSameType.hpp>
#include <boxpp/traits/Forward.hpp>

#include <boxpp/strings/Constants.hpp>
#include <boxpp/strings/Operations.hpp>
#include <boxpp/strings/Converter.hpp>

#include <boxpp/containers/Array.hpp>

namespace boxpp
{
	template<typename CharType>
	struct TNativeString
		: strings::TConstants<CharType>,
		  strings::TOperations<CharType>
	{
	};

	template<typename CharType>
	class TString
	{
	public:
		TString(u32 InitialCapacity = 0)
			: Storage(InitialCapacity)
		{
		}

		TString(const CharType* InString)
		{
			Storage.Append(InString, strings::TOperations<CharType>::Strlen(InString));
			Storage.Add(strings::TConstants<CharType>::Null);
		}

		template<typename OtherType>
		TString(const OtherType* InString)
		{
			TStringConverter<OtherType, CharType> Converter(InString);

			if (Converter.GetConvertedString())
				Append(Converter.GetConvertedString());
		}

		TString(const TString<CharType>& Other)
			: Storage(Other.Storage)
		{
		}

		TString(TString<CharType>&& Other)
			: Storage(Forward<TArray<CharType>>(Other.Storage))
		{
		}

		~TString() { }

	protected:
		TArray<CharType> Storage;

	public:
		FASTINLINE operator bool() const { return Storage.GetSize() > 1; }
		FASTINLINE bool operator !() const { return Storage.GetSize() <= 1; }
		FASTINLINE size_t GetSize() const { return *this ? Storage.GetSize() - 1 : 0; }
		FASTINLINE CharType* GetRaw() const { return *this ? Storage.GetRaw() : nullptr; }

	public:
		FASTINLINE TString<CharType>& operator =(const TString<CharType>& Other) { Storage = Other.Storage; return *this; }
		FASTINLINE TString<CharType>& operator =(TString<CharType>&& Other) { Storage = Forward<TArray<CharType>>(Other.Storage); return *this; }

	public:
		FASTINLINE ssize_t Compare(const TString<CharType>& Other) const {
			return strings::TOperations<CharType>::Strcmp(GetRaw(), Other.GetRaw());
		}

		FASTINLINE ssize_t Compare(const TString<CharType>& Other, size_t Max) const {
			return strings::TOperations<CharType>::Strncmp(GetRaw(), Other.GetRaw(), Max);
		}

		template<typename OtherType>
		FASTINLINE s32 Compare(const OtherType* String) const {
			if (IsSameType<CharType, OtherType>)
				return strings::TOperations<CharType>::Strcmp(
					(const CharType*)GetRaw(), (const OtherType*) String);

			else {
				TStringConverter<CharType, OtherType> Converted(String);
				return strings::TOperations<CharType>::Strcmp(
					(const CharType*)GetRaw(), Converted.GetConvertedString());
			}
		}

		template<typename OtherType>
		FASTINLINE s32 Compare(const OtherType* String, size_t Max) const {
			if (IsSameType<CharType, OtherType>)
				return strings::TOperations<CharType>::Strncmp(
				(const CharType*)GetRaw(), (const OtherType*)String, Max);

			else {
				TStringConverter<CharType, OtherType> Converted(String);
				return strings::TOperations<CharType>::Strncmp(
					(const CharType*)GetRaw(), Converted.GetConvertedString(), Max);
			}
		}

	public:
		FASTINLINE bool operator ==(const TString<CharType>& Other) const { return !Compare(Other); }
		FASTINLINE bool operator !=(const TString<CharType>& Other) const { return Compare(Other); }
		FASTINLINE bool operator <=(const TString<CharType>& Other) const { return Compare(Other) <= 0; }
		FASTINLINE bool operator >=(const TString<CharType>& Other) const { return Compare(Other) >= 0; }
		FASTINLINE bool operator < (const TString<CharType>& Other) const { return Compare(Other) < 0; }
		FASTINLINE bool operator > (const TString<CharType>& Other) const { return Compare(Other) > 0; }

		/* -- Below overloads are for preventing overheads. -- */
		FASTINLINE bool operator ==(const ansi_t* Other) const { return !Compare(Other); }
		FASTINLINE bool operator !=(const ansi_t* Other) const { return Compare(Other); }
		FASTINLINE bool operator <=(const ansi_t* Other) const { return Compare(Other) <= 0; }
		FASTINLINE bool operator >=(const ansi_t* Other) const { return Compare(Other) >= 0; }
		FASTINLINE bool operator < (const ansi_t* Other) const { return Compare(Other) < 0; }
		FASTINLINE bool operator > (const ansi_t* Other) const { return Compare(Other) > 0; }
		FASTINLINE bool operator ==(const wide_t* Other) const { return !Compare(Other); }
		FASTINLINE bool operator !=(const wide_t* Other) const { return Compare(Other); }
		FASTINLINE bool operator <=(const wide_t* Other) const { return Compare(Other) <= 0; }
		FASTINLINE bool operator >=(const wide_t* Other) const { return Compare(Other) >= 0; }
		FASTINLINE bool operator < (const wide_t* Other) const { return Compare(Other) < 0; }
		FASTINLINE bool operator > (const wide_t* Other) const { return Compare(Other) > 0; }

	public:
		FASTINLINE void Clear()
		{
			Storage.Clear();
		}

		FASTINLINE void Append(const TString<CharType>& Other)
		{
			if (*this) {
				Storage.RemoveAt(Storage.GetSize() - 1, 1, false);
				Storage.Insert(Storage.GetSize() - 1, Other);
			}

			else Storage.Append(Other.Storage);
		}

		FASTINLINE void Append(TString<CharType>&& Other)
		{
			if (*this) {
				Storage.RemoveAt(Storage.GetSize() - 1, 1, false);
				Storage.Insert(Storage.GetSize() - 1, Forward<TArray<CharType>>(Other.Storage));
			}

			else Storage.Append(Forward<TArray<CharType>>(Other.Storage));
		}

		FASTINLINE void Append(const CharType* InString, ssize_t MaxSize = -1)
		{
			if (InString) {
				size_t Size = strings::TOperations<CharType>::Strlen(InString);

				if (*this) {
					Storage.RemoveAt(Storage.GetSize() - 1, 1, false);
				}

				Storage.Append(InString, MaxSize >= 0 && Size > MaxSize ? MaxSize : Size);
				Storage.Add(strings::TConstants<CharType>::Null);
			}
		}

		template<typename OtherType>
		FASTINLINE void Append(const OtherType* InString, ssize_t MaxSize = -1)
		{
			if (InString) {
				TStringConverter<CharType, OtherType> Converter(InString);
				Append(Converter.GetConvertedString(), MaxSize);
			}
		}

		FASTINLINE void RemoveAt(size_t Offset, ssize_t Count = -1)
		{
			if (*this && Offset < Storage.GetSize() - 1) {
				Storage.RemoveAt(Offset, Count);
			}
		}

	public:
		FASTINLINE bool Substring(TString<CharType>& OutString, size_t Offset, ssize_t Count = -1)
		{
			OutString.Clear();

			if (*this && Offset < Storage.GetSize() - 1)
			{
				ssize_t RealMax = ssize_t(GetSize()) - Offset;

				Count = Count >= 0 && (RealMax >= Count) ? Count : RealMax;
				OutString.Append(GetRaw() + Offset, Count);

				return true;
			}

			return false;
		}

		FASTINLINE TString<CharType>& Substring(size_t Offset, ssize_t Count = -1)
		{
			TString<CharType> RetVal;
			Substring(RetVal, Offset, Count);
			return RetVal;
		}
	};

#if PLATFORM_NATIVE_WCHAR
	typedef TString<char_t> FString;
#else
	typedef TString<ansi_t> FString;
#endif
}