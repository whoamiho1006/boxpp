#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	namespace cli
	{
		class FControlCharacter
		{
		public:
			struct RawCode { s8 _; };

		public:
			FASTINLINE FControlCharacter() : Code({ 0 }) { }
			FASTINLINE FControlCharacter(const RawCode& Code) : Code(Code) { }

		public:
			static constexpr RawCode LineFeed = { '\n' };
			static constexpr RawCode CarrageReturn = { '\r' };
			static constexpr RawCode Tab = { '\t' };

			/* Non-Standard: Implemented internally. */
			static constexpr RawCode Reset = { 0x00 };

			static constexpr RawCode NoEcho = { 0x0e /* = SO */};
			static constexpr RawCode Echo = { 0x0f   /* = SI */};

		public:
			FControlCharacter(const FControlCharacter&) = default;
			FControlCharacter(FControlCharacter&&) = default;

		public:
			FControlCharacter& operator =(const FControlCharacter&) = default;
			FControlCharacter& operator =(FControlCharacter&&) = default;

		public:
			FASTINLINE FControlCharacter& operator =(const RawCode& Code) { this->Code._ = Code._; return *this; }

		public:
			FASTINLINE bool operator ==(const FControlCharacter& Other) const { return Code._ == Other.Code._; }
			FASTINLINE bool operator !=(const FControlCharacter& Other) const { return Code._ != Other.Code._; }

		private:
			RawCode Code;

		public:
			FASTINLINE const RawCode& GetRawCode() const { return Code; }
		};

		/* Fastest Shortcuts of control characters. */
		constexpr FControlCharacter::RawCode EOL = FControlCharacter::LineFeed;
		constexpr FControlCharacter::RawCode TAB = FControlCharacter::Tab;
		constexpr FControlCharacter::RawCode RST = FControlCharacter::Reset;

		constexpr FControlCharacter::RawCode NEC = FControlCharacter::NoEcho;
		constexpr FControlCharacter::RawCode ECH = FControlCharacter::Echo;
	}
}