#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/strings/String.hpp>

#include <boxpp/cli/TTYColor.hpp>
#include <boxpp/cli/ControlCharacter.hpp>

namespace boxpp
{
	namespace cli
	{
		template<bool bFg = true>
		struct TTYColorMonoInput
		{
			FASTINLINE TTYColorMonoInput(const FTTYColor& Val)
				: Color(Val)
			{
			}

			FASTINLINE TTYColorMonoInput(FTTYColor&& Val)
				: Color(Forward<FTTYColor>(Val))
			{
			}

			FTTYColor Color;
		};

		struct TTYColorStereoInput
		{
			FASTINLINE TTYColorStereoInput(const FTTYColor& Fore, const FTTYColor& Back)
				: Fore(Fore), Back(Back)
			{
			}

			FTTYColor Fore;
			FTTYColor Back;
		};

		struct TTYCoord {
			FASTINLINE TTYCoord(u16 X, u16 Y) 
				: X(X), Y(Y)
			{
			}

			u16 X, Y;
		};
		
		/*
			TTY Device.
			Usage:

			FTTY ttyOut = FTTY::Output;
			ttyOut << "TEXT" << EOL;
			...
		*/
		class BOXPP FTTY
		{
		public:
			struct TTYType { s8 _; };

		public:
			static constexpr TTYType Null = { 0 };
			static constexpr TTYType Output = { 1 };
			static constexpr TTYType Error = { 2 };

		public:
			FTTY(const TTYType& Type = Output);
			FTTY(FILE* fIn, FILE* fOut);

			/* FTTY object will not close inner streams, so use default. */
			FTTY(const FTTY& Other) = default;
			FTTY(FTTY&& Other) = default;

		public:
			FASTINLINE ~FTTY() { }

		public:
			bool IsLive();

		public:
			FTTY& operator =(const FTTY&) = default;
			FTTY& operator =(FTTY&&) = default;

		private:
			FILE* fInput;
			FILE* fOutput;

			FTTYColor Foreground;
			FTTYColor Background;

			bool bEcho;

		public:
			FTTY& operator <<(const ansi_t* InString);
			FTTY& operator <<(const wide_t* InString);

			FASTINLINE FTTY& operator <<(const FAnsiString& InString) {
				if (InString) {
					return *this << InString.GetRaw();
				}

				return *this;
			}

			FASTINLINE FTTY& operator <<(const FWideString& InString) {
				if (InString) {
					return *this << InString.GetRaw();
				}

				return *this;
			}

			// inline constant trap back.
			FASTINLINE FTTY& operator <<(s64&& Integer) {
				char_t Buffer[32] = { 0, };
				TNativeString<char_t>::Ltoa(Integer, Buffer);
				return *this << Buffer;
			}

			// inline constant trap back.
			FASTINLINE FTTY& operator <<(const f64& Number) {
				char_t Buffer[48] = { 0, };
				TNativeString<char_t>::Dtostr(Number, Buffer, 48);
				return *this << Buffer;
			}


			// inline constant trap back.
			FASTINLINE FTTY& operator <<(const f32& Number) {
				char_t Buffer[48] = { 0, };
				TNativeString<char_t>::Ftostr(Number, Buffer, 48);
				return *this << Buffer;
			}

			/*
				Integer to String.
				Conversion buffer size: 2: 8, 4: 16, 8: 32
					unsigned					signed
				16: 0 ~ 65536					-32767 ~ 32768
				32: 0 ~ 4294967296				-2147483647 ~ 2147483648
				64: 0 ~ 18446744073709551614	-9223372036854775806 ~ 9223372036854775807
			*/

#define BOXPP_TTY_INTEGER_OUTPUT_OPERATOR___SIGNED(Type) \
			FASTINLINE FTTY& operator <<(const Type& Integer) { \
				char_t Buffer[sizeof(Type) * 4] = { 0, }; \
				TNativeString<char_t>::Ltoa(Integer, Buffer); \
				return *this << Buffer; \
			}

#define BOXPP_TTY_INTEGER_OUTPUT_OPERATOR_UNSIGNED(Type) \
			FASTINLINE FTTY& operator <<(const Type& Integer) { \
				char_t Buffer[sizeof(Type) * 4] = { 0, }; \
				TNativeString<char_t>::Ultoa(Integer, Buffer); \
				return *this << Buffer; \
			}

			// s8 and u8 cannot be supported because of ansi_t.
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR___SIGNED(s64)
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR___SIGNED(s32)
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR___SIGNED(s16)
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR_UNSIGNED(u64)
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR_UNSIGNED(u32)
			BOXPP_TTY_INTEGER_OUTPUT_OPERATOR_UNSIGNED(u16)

#undef BOXPP_TTY_INTEGER_OUTPUT_OPERATOR___SIGNED
#undef BOXPP_TTY_INTEGER_OUTPUT_OPERATOR_UNSIGNED

		private:
			FTTY& SetForeground(const FTTYColor& Color) { Foreground = Color; return *this; }
			FTTY& SetBackground(const FTTYColor& Color) { Background = Color; return *this; }

		public:
			FASTINLINE FTTY& operator <<(const FTTYColor& Color) { return SetForeground(Color); }
			FASTINLINE FTTY& operator <<(const TTYColorMonoInput<true>& Color) { return SetForeground(Color.Color); }
			FASTINLINE FTTY& operator <<(const TTYColorMonoInput<false>& Color) { return SetBackground(Color.Color); }
			FASTINLINE FTTY& operator <<(const TTYColorStereoInput& Color) { 
				return SetForeground(Color.Fore).SetBackground(Color.Back); 
			}

		public:
			FTTY& operator <<(FControlCharacter&& InCharacter);
			FTTY& operator <<(const TTYCoord& Coord);

			FASTINLINE FTTY& operator <<(const FControlCharacter& InCharacter) {
				FControlCharacter Off = InCharacter;
				return (*this << Forward<FControlCharacter>(Off));
			}

		public:
			FTTY& operator >>(ansi_t& OutChar);
			FTTY& operator >>(wide_t& OutChar);

			FTTY& operator >>(FAnsiString& OutString);
			FTTY& operator >>(FWideString& OutString);

			FTTY& operator >>(s64& OutInt64);
			FTTY& operator >>(s32& OutInt32);
			FTTY& operator >>(s16& OutInt16);
		};

		/* Set foreground color. */
		FASTINLINE TTYColorMonoInput<true> Fore(const FTTYColor& Color) { return Color; }
		FASTINLINE TTYColorMonoInput<true> Fore(FTTYColor&& Color) { return Forward<FTTYColor>(Color); }

		FASTINLINE TTYColorMonoInput<false> Back(const FTTYColor& Color) { return Color; }
		FASTINLINE TTYColorMonoInput<false> Back(FTTYColor&& Color) { return Forward<FTTYColor>(Color); }

		FASTINLINE TTYColorStereoInput Color(const FTTYColor& Fore, const FTTYColor& Back) { 
			return TTYColorStereoInput(Fore, Back);
		}

		FASTINLINE TTYCoord Goto(u16 X, u16 Y) { return TTYCoord(X, Y); }

	}
}