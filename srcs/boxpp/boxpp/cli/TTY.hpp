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