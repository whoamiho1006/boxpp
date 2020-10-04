#include "CommandLine.hpp"

namespace boxpp {
	namespace cli {
		void FCommandLine::PutArgs(const ansi_t* const* argv, s32 argc)
		{
			using NS = TNativeString<ansi_t>;
			s32 i = 0;

			if (!ExecName) {
				ExecName = argv[i++];
			}

			for (; i < argc; ++i) {
				ssize_t Eq = NS::OffsetOf(argv[i], '=');

				if (Eq < 0)
					Arguments.Enqueue(argv[i]);

				else {
					FAnsiString S = argv[i];
					FAnsiString Key = S.Substring(0, Eq);

					Key.Trim();

					Arguments.Enqueue(Key);
					Arguments.Enqueue(S.Substring(Eq + 1));
				}
			}
		}

		void FCommandLine::PutArgs(const wide_t* const* argv, s32 argc)
		{
			using NS = TNativeString<wide_t>;
			s32 i = 0;

			if (!ExecName) {
				ExecName = argv[i++];
			}

			for (; i < argc; ++i) {
				ssize_t Eq = NS::OffsetOf(argv[i], '=');

				if (Eq < 0)
					Arguments.Enqueue(argv[i]);

				else {
					FAnsiString S = argv[i];
					FAnsiString Key = S.Substring(0, Eq);

					Key.Trim();

					Arguments.Enqueue(Key);
					Arguments.Enqueue(S.Substring(Eq + 1));
				}
			}
		}

		template<typename CharType>
		FASTINLINE size_t CMDLINE_LengthTo(const CharType* Mark, const CharType* Current) {
			return (((size_t)Current) - ((size_t)Mark)) / sizeof(CharType);
		}

		template<typename CharType>
		FASTINLINE void CMDLINE_PutLine(TQueue<FString>& Queue, const CharType* Line) {
			if (Line) {
				//const CharType* Mark = Line;
				CharType EscapeState = '\0';
				CharType QuatoState = '\0';
				CharType LatestCharacter = '\0';

				FString Buffer;
				Buffer.SetMultiplier(4);
				
				while (*Line) {
					if (!QuatoState) {
						EscapeState = '\0';


						switch (*Line) {
						case '\"':
						case '\'':
							QuatoState = *Line;
							break;

						case ' ':
						case '=':
							if (Buffer) {
								if (LatestCharacter != *Line)
									Queue.Enqueue(Buffer);

								Buffer.Clear();
							}

							//Queue.Enqueue(FString(Mark, CMDLINE_LengthTo(Mark, Line)));
							//Mark = Line + 1;
							break;

						default:
							Buffer.Append(*Line);
							break;
						}

					}

					else if (QuatoState == *Line) {
						if (EscapeState) 
							Buffer.Append(*Line);

						else QuatoState = '\0';
						EscapeState = '\0';
					}

					else if (EscapeState) {
						switch (*Line) {
						case ' ': Buffer.Append(' '); break;
						case 't': Buffer.Append('\t'); break;
						case 'r': Buffer.Append('\r'); break;
						case 'n': Buffer.Append('\n'); break;
						case '\\': Buffer.Append(*Line); break;
						default: break;
						}

						EscapeState = '\0';
					}

					else if (*Line == '\\') {
						EscapeState = *Line;
					}

					else Buffer.Append(*Line);

					*LatestCharacter = *Line;
					++Line;
				}

				if (Buffer) {
					Queue.Enqueue(Buffer);
					Buffer.Clear();
				}
			}
		}

		void FCommandLine::PutLine(const ansi_t* Line) {
			CMDLINE_PutLine(Arguments, Line);
		}

		void FCommandLine::PutLine(const wide_t* Line) {
			CMDLINE_PutLine(Arguments, Line);
		}

		bool FCommandLine::Invoke()
		{
			bool bKeepParsing = true;
			bError = false;

			while (!Arguments.IsEmpty() &&
					bKeepParsing)
			{
				FString Arg = *Arguments.Peek();
				Arguments.Dequeue();

				LatestArgset = nullptr; LatestArg = nullptr;
				LatestShortcut = LatestSwitch = FName::Unnamed;

				if (Arg.StartsWith("--")) {
					LatestSwitch = Arg.GetRaw() + 2;
					bError = true;

					for (FArgumentSet& ArgSet : ArgumentSets) {
						LatestArgset = &ArgSet;

						if (ArgSet.GetSwitch() == LatestSwitch) {
							LatestShortcut = ArgSet.GetShortcut();
							LatestArg = ArgSet.Invoke(Arguments);

							bError = LatestArg != nullptr || ArgSet.HasUserError();
							bUserError = ArgSet.HasUserError();

							if (LatestArg || ArgSet.HasUserError())
								bKeepParsing = false;

							break;
						}
					}
					
					break;
				}

				else if (Arg.StartsWith("-")) {
					LatestShortcut = Arg.GetRaw() + 2;
					bError = true;

					for (FArgumentSet& ArgSet : ArgumentSets) {
						LatestArgset = &ArgSet;

						if (ArgSet.GetShortcut() == LatestShortcut) {
							LatestSwitch = ArgSet.GetSwitch();
							LatestArg = ArgSet.Invoke(Arguments);

							bError = LatestArg != nullptr || ArgSet.HasUserError();
							bUserError = ArgSet.HasUserError();

							if (LatestArg || ArgSet.HasUserError())
								bKeepParsing = false;

							break;
						}
					}

					break;
				}

				else {
					TextArguments.Enqueue(Arg);
				}
			}

			if (!bError) {
				LatestSwitch = FName::Unnamed;
				LatestShortcut = FName::Unnamed;

				LatestArgset = &DefaultSet;
				LatestArg = DefaultSet.Invoke(TextArguments);

				bError = LatestArg != nullptr || DefaultSet.HasUserError();
				bUserError = DefaultSet.HasUserError();
			}

			return !bError;
		}
	}
}