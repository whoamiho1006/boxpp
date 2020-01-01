#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/cli/Arguments.hpp>

namespace boxpp {
	namespace cli {
		/*

			Command line rules:

			1. Plain text arg.			(text)
			2. Switch arg				(-s)
			3. Option-Value arg.
				1) full name	(--option value)
				2) shortcut		(-o value)

			e.g.
			test -v -h test good --name=jay

		*/
		class BOXPP FCommandLine
		{
		public:
			FCommandLine() {

			}

			~FCommandLine() {

			}

		private:
			FString ExecName;
			TQueue<FString> Arguments;
			TLinkedList<FArgumentSet> ArgumentSets;

		public:
			void PutArgs(const ansi_t* const* argv, s32 argc);
			void PutArgs(const wide_t* const* argv, s32 argc);

			void PutLine(const ansi_t* Line);
			void PutLine(const wide_t* Line);

		public:
			bool Invoke();
		};

	}
}