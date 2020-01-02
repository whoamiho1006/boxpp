#include "Arguments.hpp"

namespace boxpp {
	namespace cli {
		IArgument* FArgumentSet::Test(TQueue<FString>& InArguments) const
		{
			size_t N = 0;

			auto RIter = Requirements.Begin();
			while (RIter && InArguments.GetSize() > N) {
				const FString& SArg = *InArguments.Peek(N);

				if (IArgument* Arg = (*RIter).Argument) {
					if (!Arg->CanParse(SArg))
						return Arg;
				}

				++N;
				++RIter;
			}

			if (RIter) {
				return (*RIter).Argument;
			}

			return nullptr;
		}

		IArgument* FArgumentSet::Invoke(TQueue<FString>& InArguments) const
		{
			IArgument* Impossible = Test(InArguments);

			if (!Impossible) {
				size_t N = 0;

				auto RIter = Requirements.Begin();

				while (RIter) {
					FString SArg = *InArguments.Peek();
					InArguments.Dequeue();

					if (IArgument* Arg = (*RIter).Argument) {
						Arg->Parse(SArg);
					}

					++RIter;
				}
				
				RIter = Requirements.Begin();

				while (RIter) {
					if ((*RIter).Callback) {
						(*RIter).Callback((*RIter).Argument);
					}

					++RIter;
				}

				if (Callback) {
					Callback(const_cast<FArgumentSet*>(this));
				}
			}

			else if (ErrorCallback) {
				ErrorCallback(const_cast<FArgumentSet*>(this), Impossible);
			}

			return Impossible;
		}

		FArgumentSet& FArgumentSet::_Requires(IArgument* Argument, const FName& Name, const FString& Usage)
		{
			Requirement* Req = Requirements.Add(Requirement())->GetRaw();
			
			Req->Argument = Argument;
			Req->Name = Name;
			Req->Usage = Usage;

			return *this;
		}

		FArgumentSet& FArgumentSet::_Requires(IArgument* Argument, TFunction<void(IArgument*)>&& Callback, const FName& Name, const FString& Usage)
		{
			Requirement* Req = Requirements.Add(Requirement())->GetRaw();

			Req->Argument = Argument;
			Req->Callback = Forward<TFunction<void(IArgument*)>>(Callback);
			Req->Name = Name;
			Req->Usage = Usage;

			return *this;
		}

		bool FNumericArgument::TryParse(const CharType* InString, bool bEmulateOnly) const {
			static const CharType Digits[] = {
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 0
			};

			static const CharType DigitsBig[] = {
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 0
			};

			using Native = TNativeString<CharType>;
			const CharType* Original = InString;

			s32 ValueType = -1;

			if (InString) {
				s32 Radix = 0; /* 8, 10, 16, 32: floating, 64: exponent. */
				while (*InString &&
					(*InString == ' ' || *InString == '\t'))
					++InString;

				/*
				Integer rules:
					1. Radix:  8. --> 0[1-9][0-9]+.
					2. Radix: 10. --> [+-]?[1-9][0-9]+.
					3. Radix: 16. --> 0x[0-9a-fA-F]+.

				Float rules:
					1. Integer part: [+-]?(\.|[1-9][0-9]+\.) --> covered by radix 10.
					2. Prime part: [0-9]+(e[+-]?[1-9][0-9]*)
				*/

				CharType Temp = *(InString++);
				bool bExpSign = false;

				// [+-]+		Radix 10 / Floating.
				if (Temp == '+' || Temp == '-') {
					ValueType = 0;
					Radix = 10;
				}

				// 0|0x
				else if (Temp == '0') {
					ValueType = 1;
					Radix = *(++InString) == 'x' ? 16 : 8;

					/* If string terminated in here, its radix is 10. */
					if (*InString == 0) {
						Radix = 10;
					}
				}

				/* [1-9]		Radix 10 / Floating.*/
				else if (Native::Contains(Digits + 1, Temp, 9)) {
					ValueType = 0;
					Radix = 10;
				}

				while ((Temp = *(InString++)) != 0 && Temp) {
					// Dot (.) appears: \.
					// --> floating.

					// \.|[1-9][0-9]+\.
					if ((ValueType < 0 || Radix == 10) &&
						(Temp == '.' || Temp == 'e'))
					{
						ValueType = 2;
						Radix = Temp == 'e' ? 64 : 32;
						continue;
					}

					// .... e?
					else if (ValueType == 2 &&
						Radix == 32 && Temp == 'e')
					{
						Radix = 64;
						continue;
					}

					switch (Radix) {
					case 8:
						if (!Native::Contains(Digits, Temp, 8)) {
							return false;
						}
						break;

					case 10:
						if (!Native::Contains(Digits, Temp, 10)) {
							return false;
						}

						break;

					case 16:
						if (!Native::Contains(Digits, Temp) &&
							!Native::Contains(DigitsBig, Temp))
						{
							return false;
						}
						break;

					case 32:
						// [0-9]+
						if (!Native::Contains(Digits, Temp, 10))
						{
							return false;
						}
						break;

					case 64:
						if (!bExpSign) {
							// [+|-]?
							if ((Temp == '+' || Temp == '-') ||
								Native::Contains(Digits, Temp, 10))
								bExpSign = true;

							else {
								return false;
							}
						}

						// [0-9]+
						else if (!Native::Contains(Digits, Temp, 10))
							return false;

						break;
					}
				}

				if (!bEmulateOnly && ValueType >= 0) {
					FNumericArgument* This = const_cast<FNumericArgument*>(this);

					This->ValueType = ValueType;
					switch (ValueType) {
					case 0:
						This->SignedValue = Native::Atol(Original, Radix);
						break;

					case 1:
						This->SignedValue = Native::Atoul(Original, Radix);
						break;

					case 2:
						This->DoubleValue = Native::Strtod(Original);
						break;
					}
				}
			}

			return ValueType >= 0;
		}
}
}