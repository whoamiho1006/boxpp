#include "TTY.hpp"

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#define cross_isatty _isatty
#define cross_fileno _fileno
#else
#include <unistd.h>
#define cross_isatty isatty
#define cross_fileno fileno
#endif

namespace boxpp
{
	namespace cli
	{
		struct FTTYColorState
		{
			FTTYColor Fg;
			FTTYColor Bg;
		};

		struct FTTYState
		{
			FTTYColorState Colors;

#if PLATFORM_POSIX
			termios TermMode;
#endif
#if PLATFORM_WINDOWS
			DWORD dwInputMode;

			HANDLE hStdOut;
			HANDLE hStdErr;
			HANDLE hStdIn;
#endif
		};

		FTTYState& GetTTYState() {
			static FTTYState _State;
			static bool _HasInit = false;

			if (!_HasInit) {
				_HasInit = true;

				_State.Colors.Fg = FTTYColor::LightGray;
				_State.Colors.Fg = FTTYColor::LightGray;

#if PLATFORM_POSIX
				tcgetattr(cross_fileno(stdin), &_State.TermMode);
#endif

#if PLATFORM_WINDOWS
				DWORD dwMode = 0;
				
				_State.hStdIn = GetStdHandle(STD_INPUT_HANDLE);
				_State.hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
				_State.hStdErr = GetStdHandle(STD_ERROR_HANDLE);

				GetConsoleMode(_State.hStdIn, &_State.dwInputMode);
				SetConsoleMode(_State.hStdIn, _State.dwInputMode |= (ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT));

				GetConsoleMode(_State.hStdOut, &dwMode);
				SetConsoleMode(_State.hStdOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

				GetConsoleMode(_State.hStdErr, &dwMode);
				SetConsoleMode(_State.hStdErr, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
			}

			return _State;
		}

		FASTINLINE bool TTY_IsReallyTTY(FILE* fOutput) {
			return ((fOutput == stdin || fOutput == stdout || fOutput == stderr) &&
				cross_isatty(cross_fileno(stdout)));
		}

		FASTINLINE bool TTY_IsCanonicalMode() {
			bool RetVal = false;

#if PLATFORM_POSIX
			RetVal = (GetTTYState().TermMode.c_lflag & ICANON) == ICANON;
#endif
#if PLATFORM_WINDOWS
			
			RetVal = (GetTTYState().dwInputMode & 
				(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT)) ==
				(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
#endif
			return RetVal;
		}

		FASTINLINE bool TTY_IsEchoMode() {
			bool RetVal = false;

#if PLATFORM_POSIX
			RetVal = (GetTTYState().TermMode.c_lflag & ECHO) == ICANON;
#endif
#if PLATFORM_WINDOWS
			RetVal = (GetTTYState().dwInputMode & ENABLE_ECHO_INPUT) == ENABLE_ECHO_INPUT;
#endif
			return RetVal;
		}

		void TTY_SetMode(bool Canonical, bool Echo) {
			static FAtomicBarrior Atomic;
			FAtomicScope Guard(Atomic);

#if PLATFORM_POSIX
			termios& Mode = GetTTYState().TermMode;

			if (!tcgetattr(cross_fileno(stdin), &Mode)) {
				if (Canonical) {
					Mode.c_lflag |= ICANON;
				} else {
					Mode.c_lflag &= ~ICANON;
				}

				if (Echo) {
					Mode.c_lflag |= ECHO;
				}
				else {
					Mode.c_lflag &= ~ECHO;
				}

				tcsetattr(cross_fileno(stdin), TCSANOW, &Mode);
			}
#endif
#if PLATFORM_WINDOWS
			DWORD& dwMode = GetTTYState().dwInputMode;
			GetConsoleMode(GetTTYState().hStdIn, &dwMode);

			if (Canonical) {
				dwMode |= ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT;
			}
			else {
				dwMode &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
			}

			if (Echo) {
				dwMode |= ENABLE_ECHO_INPUT;
			}
			else {
				dwMode &= ~ENABLE_ECHO_INPUT;
			}

			SetConsoleMode(GetTTYState().hStdIn, dwMode);
#endif
		}

		FASTINLINE void TTY_UpdateState(FILE* fOutput, 
			const FTTYColor& Fg, const FTTYColor& Bg) 
		{
			static FAtomicBarrior Atomic;
			FAtomicScope Guard(Atomic);

			/*
				Check given stream is really TTY or not.
				And, if it is tty, make syscalls.
			*/
			if (TTY_IsReallyTTY(fOutput)) {
				FTTYState& State = GetTTYState();

				if (State.Colors.Fg != Fg || State.Colors.Bg != Bg) {
					fprintf(stdout, "\033[%dm\033[%dm",
						Fg.GetRawCode()._ + 30, Bg.GetRawCode()._ + 40);
				}

				State.Colors.Fg = Fg;
				State.Colors.Bg = Bg;
			}
		}


		FTTY::FTTY(const TTYType& Type)
			: fInput(nullptr), fOutput(nullptr), bEcho(true)
		{
			FTTYColorState* State = nullptr;

			switch (Type._) {
			case 0:
				break;

			case 1:
				fInput = stdin;
				fOutput = stdout;

				State = &(GetTTYState().Colors);
				break;

			case 2:
				fInput = stdin;
				fOutput = stderr;

				State = &(GetTTYState().Colors);
				break;

			default:
				break;
			}

			if (!State) {
				Foreground = FTTYColor::LightGray;
				Background = FTTYColor::Black;
			}

			else {
				Foreground = State->Fg;
				Background = State->Bg;
			}
		}

		FTTY::FTTY(FILE* fIn, FILE* fOut)
			: fInput(fIn), fOutput(fOut), bEcho(true)
		{
			bEcho = IsLive();
		}

		bool FTTY::IsLive()
		{
			if (fOutput) {
				return TTY_IsReallyTTY(fOutput);
			}

			return false;
		}

		FTTY& FTTY::operator<<(const ansi_t* InString) {
			if (fOutput) {
				TTY_UpdateState(fOutput, Foreground, Background);
				fprintf(fOutput, "%s", InString);
			}

			return *this;
		}

		FTTY& FTTY::operator<<(const wide_t* InString) {
			if (fOutput) {
				TTY_UpdateState(fOutput, Foreground, Background);

#if PLATFORM_WINDOWS
				fprintf(fOutput, "%S", InString);
#else
				fprintf(fOutput, "%S", InString);
#endif

				fflush(fOutput);
			}

			return *this;
		}

		FTTY& FTTY::operator<<(FControlCharacter&& InCharacter) {
			if (fOutput) {
				TTY_UpdateState(fOutput, Foreground, Background);

				if (InCharacter == EOL) {
					fprintf(fOutput, "\n");
				}

				else if (InCharacter == FControlCharacter::CarrageReturn) {
					fprintf(fOutput, "\r");
				}

				else if (InCharacter == TAB) {
					fprintf(fOutput, "\t");
				}

				else if (InCharacter == RST) {
					// Only for real-tty.

					Foreground = FTTYColor::LightGray;
					Background = FTTYColor::Black;

					if (TTY_IsReallyTTY(fOutput)) {
						fprintf(stdout, "\033[2J\033[1;1H");
						*this << Goto(0, 0);
					}
				}

				else if (InCharacter == NEC) {
					bEcho = false;
				}

				else if (InCharacter == ECH) {
					bEcho = true;
				}


				fflush(fOutput);
			}

			return *this;
		}

		FTTY& FTTY::operator<<(const TTYCoord& Coord) {
			if (fOutput) {
				if (TTY_IsReallyTTY(fOutput)) {
					fprintf(stdout, "\x1b[%d;%df", Coord.Y, Coord.X);
					fprintf(stderr, "\x1b[%d;%df", Coord.Y, Coord.X);
				}
			}

			return *this;
		}

		struct TTYCharModeState
		{
			bool bApply, bCanonical, bEcho;
		};

		FASTINLINE void TTY_EnterCharMode(FILE* fInput,
			TTYCharModeState& State, bool bNewC, bool bNewEcho)
		{
			State.bApply = false;

			if (fInput && TTY_IsReallyTTY(fInput)) {
				State.bCanonical = TTY_IsCanonicalMode();
				State.bEcho = TTY_IsEchoMode();

				if (State.bCanonical != bNewC ||
					State.bEcho != bNewEcho)
				{
					State.bApply = true;
					TTY_SetMode(bNewC, bNewEcho);
				}
			}
		}

		FASTINLINE void TTY_LeaveCharMode(const TTYCharModeState& State) {
			if (State.bApply) {
				TTY_SetMode(State.bCanonical, State.bEcho);;
			}
		}

		FTTY & FTTY::operator>>(ansi_t& OutChar)
		{
			TTYCharModeState State;
			TTY_EnterCharMode(fInput, State, true, bEcho);

			OutChar = 0;

			if (fInput) {
				fflush(fInput);

				int R = fgetc(fInput);
				if (R) OutChar = (ansi_t)R;
			}

			TTY_LeaveCharMode(State);
			return *this;
		}

		FTTY& FTTY::operator>>(wide_t& OutChar)
		{
			TTYCharModeState State;
			TTY_EnterCharMode(fInput, State, true, bEcho);

			OutChar = 0;

			if (fInput) {
				fflush(fInput);

				wint_t R = fgetwc(fInput);
				if (R) OutChar = (wide_t)R;
			}

			TTY_LeaveCharMode(State);
			return *this;
		}

		FTTY& FTTY::operator>>(FAnsiString& OutString)
		{
			ansi_t Char = 0;

			while (true)
			{
				*this >> Char;
				
				if (Char == '\0' || Char == '\n')
				{
					break;
				}

				else if (Char == '\r')
					continue;

				else {
					OutString.Append(Char);
				}
			}

			return *this;
		}

		FTTY& FTTY::operator>>(FWideString& OutString)
		{
			wide_t Char = 0;

			while (true)
			{
				*this >> Char;
				
				if (Char == L'\0' || Char == L'\n')
				{
					break;
				}

				else if (Char == L'\r')
					continue;

				else {
					OutString.Append(Char);
				}
			}

			return *this;
		}

		FTTY& FTTY::operator>>(s64& OutInt64)
		{
			return *this;
		}

		FTTY& FTTY::operator>>(s32& OutInt32)
		{
			return *this;
		}

		FTTY & FTTY::operator>>(s16& OutInt16)
		{
			return *this;
		}
	}
} 