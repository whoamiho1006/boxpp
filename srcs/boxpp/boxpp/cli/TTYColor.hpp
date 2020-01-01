#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	namespace cli
	{
		class FTTYColor
		{
		public:
			struct RawCode { u8 _; };

		public:
			FASTINLINE FTTYColor() : Code({ 0 }) { }
			FASTINLINE FTTYColor(const RawCode& Code) : Code(Code) { }

		public:
			static constexpr RawCode Black = { 0 };
			static constexpr RawCode Red = { 1 };
			static constexpr RawCode Green = { 2 };
			static constexpr RawCode Yellow = { 3 };
			static constexpr RawCode Blue = { 4 };
			static constexpr RawCode Magenta = { 5 };
			static constexpr RawCode Cyan = { 6 };
			static constexpr RawCode LightGray = { 7 };

			static constexpr RawCode Gray = { 0 | (0x80) };
			static constexpr RawCode LightRed = { 1 | (0x80) };
			static constexpr RawCode LightGreen = { 2 | (0x80) };
			static constexpr RawCode LightYellow = { 3 | (0x80) };
			static constexpr RawCode LightBlue = { 4 | (0x80) };
			static constexpr RawCode LightMagenta = { 5 | (0x80) };
			static constexpr RawCode LightCyan = { 6 | (0x80) };
			static constexpr RawCode White = { 7 | (0x80) };

		public:
			FTTYColor(const FTTYColor&) = default;
			FTTYColor(FTTYColor&&) = default;

		public:
			FTTYColor& operator =(const FTTYColor&) = default;
			FTTYColor& operator =(FTTYColor&&) = default;

		public:
			FASTINLINE FTTYColor& operator =(const RawCode& Code) { this->Code._ = Code._; return *this; }

		public:
			FASTINLINE bool operator ==(const FTTYColor& Other) const { return Code._ == Other.Code._; }
			FASTINLINE bool operator !=(const FTTYColor& Other) const { return Code._ != Other.Code._; }

		private:
			RawCode Code;

		public:
			FASTINLINE const RawCode& GetRawCode() const { return Code; }
		};
	}
}