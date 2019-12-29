#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/String.hpp>

namespace boxpp
{
	/*
		Name for identifying modules, objects...
	*/
	class FName
	{
	private:
		struct NameData {
			u32 Length;
			s32 References;
		};

	public:
		struct UNNAMED { };

	public:
		constexpr FName() : Name(nullptr) { }
		constexpr FName(nullptr_t) : Name(nullptr) { }
		constexpr FName(UNNAMED) : Name(nullptr) { }

		FName(const ansi_t* Name) { Construct(Name); }
		FName(const wide_t* Name) { Construct(Name); }
		FName(const FName& Other) : Name(Other.Name) { if (Name) ++Name->References; }
		FName(FName&& Other) : Name(Other.Name) { Other.Name = nullptr; }
		~FName() { Destruct(); }

	public:
		static constexpr const UNNAMED Unnamed = { };

	public:
		FASTINLINE operator bool() const { return Name; }
		FASTINLINE bool operator !() const { return !Name; }

	public:
		FASTINLINE FName& operator =(nullptr_t) { Destruct(); return *this; }
		FASTINLINE FName& operator =(UNNAMED) { Destruct(); return *this; }
		FASTINLINE FName& operator =(const FName& Other) {
			if (this != &Other) {
				Destruct();

				if ((Name = Other.Name) != nullptr)
					++Name->References;
			}

			return *this;
		}

		FASTINLINE FName& operator =(FName&& Other) {
			if (this != &Other) {
				Destruct();
				Swap(Name, Other.Name);
			}

			return *this;
		}

		/* -- Below overloads are for preventing overheads. -- */
		FASTINLINE FName& operator =(const ansi_t* Name) { Destruct(); Construct(Name); return *this; }
		FASTINLINE FName& operator =(const wide_t* Name) { Destruct(); Construct(Name); return *this; }

	public:
		FASTINLINE u32 GetSize() const { return Name ? Name->Length : 0; }
		FASTINLINE const char_t* GetRaw() const { return Name ? (const char_t*)(Name + 1) : nullptr; }

	public:
		FASTINLINE bool operator ==(const FName& Other) const { return !Compare(Other); }
		FASTINLINE bool operator !=(const FName& Other) const { return Compare(Other); }
		FASTINLINE bool operator <=(const FName& Other) const { return Compare(Other) <= 0; }
		FASTINLINE bool operator >=(const FName& Other) const { return Compare(Other) >= 0; }
		FASTINLINE bool operator < (const FName& Other) const { return Compare(Other) < 0; }
		FASTINLINE bool operator > (const FName& Other) const { return Compare(Other) > 0; }

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
		FASTINLINE s32 Compare(const FName& Other) const {
			if (Name != Other.Name)
			{
				if (Name && Other.Name) {
					return TNativeString<char_t>::Strcmp(GetRaw(), Other.GetRaw());
				}

				return Name && !Other.Name ? -1 : 1;
			}

			return 0;
		}

		template<typename CharType>
		FASTINLINE s32 Compare(const CharType* String) const {
			if (!Name) return String ? 1 : 0;
			else if (Name && !String) {
				return -1;
			}

			else if (IsSameType<CharType, char_t>)
				return TNativeString<CharType>::Strcmp((const CharType*)(Name + 1), String);

			else {
				static FAtomicBarrior __BARRIOR;
				static TStringConverter<char_t, CharType> Converter;
				FAtomicScope __Guard(__Barrior);

				Converter.Reset(String);

				return TNativeString<char_t>::Strcmp(
					(const char_t*)(Name + 1), Converter);
			}
		}

	private:
		template<typename CharType>
		FASTINLINE void Construct(const CharType* InString)
		{
			if (IsSameType<char_t, CharType>)
			{
				if (!InString)
					Name = nullptr;

				else {
					size_t Length = TNativeString<CharType>::Strlen(InString);
					u8* Intergrated = new u8[sizeof(NameData) + (Length + 1) * sizeof(char_t)];

					(Name = (NameData*)Intergrated)
						->References = 1;

					Name->Length = u32(Length);

					::memcpy(Name + 1, InString, Length * sizeof(char_t));
					*((char_t*)(Name + 1) + Length) = 0;
				}
			}
			else {
				static FAtomicBarrior __BARRIOR;
				static TStringConverter<char_t, CharType> Converter;

				FAtomicScope __Guard(__Barrior);
				Converter.Reset(InString);

				if (!Converter)
					Name = nullptr;

				else {
					u8* Intergrated = new u8[sizeof(NameData) +
						(Converter.GetConvertedLength() + 1) * sizeof(char_t)];

					(Name = (NameData*)Intergrated)
						->References = 1;

					Name->Length = u32(Converter.GetConvertedLength());
					::memcpy(Name + 1, Converter.GetConvertedString(),
						(Converter.GetConvertedLength() + 1) * sizeof(char_t)
					/* including termination character. */);
				}
			}
		}

		FASTINLINE void Destruct() {
			if (Name && !--Name->References)
			{
				delete[]((u8*)Name);
			}

			Name = nullptr;
		}

	private:
		NameData* Name;
	};
}