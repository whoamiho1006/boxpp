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
		enum class ECmdError
		{
			Success = 0,
			UnknownSwitch,
			UnknownShortcut,
			InvalidArguments,
			InvalidDefaultArguments,
			UserErrorSet
		};

		class BOXPP FCommandLine
		{
		public:
			FCommandLine() 
				: LatestArgset(nullptr), 
				  LatestArg(nullptr), 
				  bError(false), bUserError(false),
				  DefaultSet(FName::Unnamed, FName::Unnamed)
			{

			}

			~FCommandLine() {

			}

		private:
			FString ExecName;
			TQueue<FString> Arguments;
			TQueue<FString> TextArguments;
			TLinkedList<FArgumentSet> ArgumentSets;
			FArgumentSet DefaultSet;

		private:
			FName LatestSwitch, LatestShortcut;
			FArgumentSet* LatestArgset;
			IArgument* LatestArg;
			bool bError, bUserError;

		public:
			void PutArgs(const ansi_t* const* argv, s32 argc);
			void PutArgs(const wide_t* const* argv, s32 argc);

			void PutLine(const ansi_t* Line);
			void PutLine(const wide_t* Line);

		public:
			bool Invoke();

		public:
			FASTINLINE ECmdError GetErrorCode() const {
				if (bError) {
					if (bUserError)
						return ECmdError::UserErrorSet;

					else if (LatestSwitch && !LatestShortcut)
						return ECmdError::UnknownSwitch;

					else if (!LatestSwitch && LatestShortcut)
						return ECmdError::UnknownShortcut;

					else if (!LatestSwitch && !LatestShortcut)
						return ECmdError::InvalidDefaultArguments;

					else if (LatestArg)
						return ECmdError::InvalidArguments;
				}

				return ECmdError::Success;
			}

		public:
			FASTINLINE const FName& GetLatestSwitch() const { return LatestSwitch; }
			FASTINLINE const FName& GetLatestShortcut() const { return LatestShortcut; }
			FASTINLINE FArgumentSet* GetDefaultSet() const { return const_cast<FArgumentSet*>(&DefaultSet); }
			FASTINLINE IArgument* GetLatestArgument() const { return LatestArg; }

		public:
			FASTINLINE FArgumentSet& On() { return DefaultSet; }
			FASTINLINE FArgumentSet& On(const FName& Shortcut, 
				const FName& Switch, const FString& Usage)
			{
				return *((*(ArgumentSets.Add(FArgumentSet(
					Shortcut, Switch, Usage))))->GetRaw());
			}

		public:
			FASTINLINE bool GetTextArguments(FString& OutArgument) {
				return TextArguments.Dequeue(OutArgument);
			}
		};

	}
}