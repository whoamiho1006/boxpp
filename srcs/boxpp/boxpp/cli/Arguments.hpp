#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/IsDerivedType.hpp>
#include <boxpp/base/tpls/containers/Function.hpp>
#include <boxpp/base/tpls/containers/LinkedList.hpp>
#include <boxpp/base/tpls/containers/Queue.hpp>
#include <boxpp/base/tpls/strings/String.hpp>
#include <boxpp/base/utilities/Name.hpp>

namespace boxpp
{
	namespace cli {
		/*
			Argument
		*/
		class IArgument
		{
			friend class FArgumentSet;

		public:
			IArgument() { }
			virtual ~IArgument() { }

		public:
			virtual bool CanParse(const FString& InString) const = 0;
			virtual bool Parse(const FString& InString) = 0;

		public:
			/* Get string that supplied before parsing. */
			virtual const FString& GetString() const = 0;

			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const = 0;

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const = 0;

		};

		/*
			Templated Argument.
		*/
		template<typename Type>
		class TTypedArgument : public IArgument
		{
		public:
			static constexpr bool Supported = false;

		public:
			TTypedArgument() { }
			~TTypedArgument() { }

		public:
			virtual bool CanParse(const FString& InString) const override {
				return false;
			}

			virtual bool Parse(const FString& InString) override {
				return false;
			}

		private:
			FString ValueString;
			Type Value;

		public:
			/* Get string that supplied before parsing. */
			virtual const FString& GetString() const override { return ValueString; }

			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const { return &Value; }

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const { return sizeof(Value); }
		};

		/*
			String Argument.
		*/
		class BOXPP FStringArgument : public IArgument
		{
		public:
			FStringArgument() { }
			~FStringArgument() { }

		public:
			virtual bool CanParse(const FString& InString) const override { return true; }
			virtual bool Parse(const FString& InString) override {
				Value = InString;
				return true;
			}

		private:
			FString Value;

		public:
			/* Get string that supplied before parsing. */
			virtual const FString& GetString() const override { return Value; }

			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const { return &Value; }

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const { return sizeof(Value); }
		};

		class BOXPP FNameArgument : public FStringArgument
		{
		public:
			FNameArgument() { }
			~FNameArgument() { }

		public:
			virtual bool Parse(const FString& InString) override {
				Name = *InString;
				return true;
			}

		private:
			FName Name;

		public:
			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const { return &Name; }

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const { return sizeof(Name); }
		};

		class BOXPP FBooleanArgument : public IArgument
		{
		public:
			FBooleanArgument() { }

		private:
			using ConstPicker = TPickConstant<IsSameType<FString::CharType, wide_t>, const wide_t*, const ansi_t*>;
			static constexpr const FString::CharType* TrueVal = ConstPicker::Pick(L"true", "true");
			static constexpr const FString::CharType* FalseVal = ConstPicker::Pick(L"false", "false");
			static constexpr const FString::CharType* ZeroVal = ConstPicker::Pick(L"0", "0");
			static constexpr const FString::CharType* OneVal = ConstPicker::Pick(L"1", "1");

		public:
			virtual bool CanParse(const FString& InString) const override {
				if (InString.CompareInCase(TrueVal) || InString.CompareInCase(FalseVal) ||
					InString.CompareInCase(ZeroVal) || InString.CompareInCase(OneVal))
				{
					return true;
				}

				return false;
			}

			virtual bool Parse(const FString& InString) override {
				ValueString = InString;
				Value = InString.CompareInCase(TrueVal) || InString.CompareInCase(OneVal);
				return true;
			}

		private:
			FString ValueString;
			bool Value;

		public:
			/* Get string that supplied before parsing. */
			virtual const FString& GetString() const override { return ValueString; }

			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const { return &Value; }

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const { return sizeof(Value); }
		};

		/*
			Numeric Argument.
		*/
		class BOXPP FNumericArgument : public IArgument
		{
		public:
			FNumericArgument() { }
			~FNumericArgument() { }

		public:
			typedef FString::CharType CharType;

		public:
			virtual bool CanParse(const FString& InString) const override {
				return TryParse(InString.GetRaw(), true);
			}

			virtual bool Parse(const FString& InString) override {
				return TryParse(InString.GetRaw(), false);
			}

		private:
			bool TryParse(const CharType* InString, bool bEmulateOnly) const;

		private:
			FString ValueString;

		protected:
			s32 ValueType; /* 0: signed, 1: Unsigned, 2: Double. */
			union {
				s64 SignedValue;
				u64 UnsignedValue;
				f64 DoubleValue;
			};

		public:
			FASTINLINE bool IsSigned() const { return ValueType == 0; }
			FASTINLINE bool IsUnsigned() const { return ValueType == 1; }
			FASTINLINE bool IsDouble() const { return ValueType == 2; }

		public:
			/* Get string that supplied before parsing. */
			virtual const FString& GetString() const override { return ValueString; }

			/* Get raw storage (for storing parsed value) pointer. */
			virtual const void* GetRaw() const { return &SignedValue; }

			/* Get raw storage's size. */
			virtual const s32 GetRawSize() const { return sizeof(s64) > sizeof(f64) ? sizeof(s64) : sizeof(f64); }
		};

#define BOXPP_TYPED_ARGUMENT_OVERRIDE(Typed) \
			private: Typed Conversion; \
			public: static constexpr bool Supported = true; \
			public: virtual bool Parse(const FString& InString) override { \
				if (FNumericArgument::Parse(InString)) { \
					switch (ValueType) { \
						case 0: Conversion = (Typed) (this->SignedValue); break; \
						case 1: Conversion = (Typed) (this->UnsignedValue); break; \
						case 2: Conversion = (Typed) (this->DoubleValue); break; \
					} \
					\
					return true; \
				} \
				\
				return false; \
			} \
			\
			public: virtual const void* GetRaw() const override { return &Conversion; } \
			public: virtual const s32 GetRawSize() const override { return sizeof(Conversion); }

		template<> class TTypedArgument<FString> : public FStringArgument { public: static constexpr bool Supported = true; };
		template<> class TTypedArgument<FName> : public FNameArgument { public: static constexpr bool Supported = true; };
		template<> class TTypedArgument<FBooleanArgument> : public FBooleanArgument { public: static constexpr bool Supported = true; };

		template<> class TTypedArgument<s64> : public FNumericArgument { public: static constexpr bool Supported = true; };
		template<> class TTypedArgument<s32> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(s32) };
		template<> class TTypedArgument<s16> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(s16) };
		template<> class TTypedArgument<s8> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(s8) };

		template<> class TTypedArgument<u64> : public FNumericArgument { public: static constexpr bool Supported = true; };
		template<> class TTypedArgument<u32> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(u32) };
		template<> class TTypedArgument<u16> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(u16) };
		template<> class TTypedArgument<u8> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(u8) };

		template<> class TTypedArgument<f64> : public FNumericArgument { public: static constexpr bool Supported = true; };
		template<> class TTypedArgument<f32> : public FNumericArgument { BOXPP_TYPED_ARGUMENT_OVERRIDE(f32) };

		/*
			Argument set.

			e.g. -n, --name STRING:
			FArgumentSet NameSpec ("n", "name");

		*/
		class BOXPP FArgumentSet
		{
		public:
			FArgumentSet(const FName& Shortcut, const FName& Switch, const FString& Usage = "")
				: Shortcut(Shortcut), Switch(Switch), Usage(Usage)
			{
			}

			virtual ~FArgumentSet()
			{
				for (Requirement& Req : Requirements) {
					if (Req.Argument)
						delete Req.Argument;
				}

				Requirements.Clear();
			}

		public:
			class Requirement
			{
			public:
				IArgument* Argument;
				TFunction<void(IArgument*)> Callback;

				FName Name;
				FString Usage;
			};

		private:
			FName Shortcut, Switch;
			FString Usage;

			TLinkedList<Requirement> Requirements;
			
			TFunction<void(const FArgumentSet*)> Callback;
			TFunction<void(const FArgumentSet*, IArgument*)> ErrorCallback;

		public:
			FASTINLINE const FName& GetShortcut() const { return Shortcut; }
			FASTINLINE const FName& GetSwitch() const { return Switch; }
			FASTINLINE const FString& GetUsage() const { return Usage; }

		public:
			FASTINLINE FArgumentSet& SetCallback(TFunction<void(const FArgumentSet*)> Callback) {
				this->Callback = Forward<TFunction<void(const FArgumentSet*)>>(Callback);
				return *this;
			}

			FASTINLINE FArgumentSet& SetErrorCallback(TFunction<void(const FArgumentSet*, IArgument*)> Callback) {
				this->ErrorCallback = Forward<TFunction<void(const FArgumentSet*, IArgument*)>>(Callback);
				return *this;
			}
			
		public:
			/*
				Test arguments are suitable for requirements.
				Returns argument object if error met.
				If success, returns nullptr.

				This method tests post-argument which are placed on next of switch.
				e.g. --test arg1 arg2 => arg1 and arg2 will be tested.

				Note that, this method will NOT trigger any callbacks.
			*/
			IArgument* Test(TQueue<FString>& InArguments) const;

			/*
				Consume arguments from queue and invokes all callbacks which are configured with argument.
				Note that, this method tests arguments before putting them to argument objects.

				Returns argument object if error met.
				If success, returns nullptr.

				This method tests post-argument which are placed on next of switch.
				e.g. --test arg1 arg2 => arg1 and arg2 will be tested.
			*/
			IArgument* Invoke(TQueue<FString>& InArguments) const;

		public:
			FASTINLINE const TLinkedList<Requirement>& GetRequirements() const { return Requirements; }
			
			FASTINLINE bool GetArgumentName(IArgument* Argument, FName& OutName) const {
				for (const Requirement& Req : Requirements) {
					if (Req.Argument == Argument) {
						OutName = Req.Name;
						return true;
					}
				}

				OutName = FName::Unnamed;
				return false;
			}

			FASTINLINE bool GetArgumentUsage(IArgument* Argument, FString& OutUsage) const {
				for (const Requirement& Req : Requirements) {
					if (Req.Argument == Argument) {
						OutUsage.Append(Req.Usage);
						return true;
					}
				}

				return false;
			}

		public:
			/*
				Adds custom requirement.
				Note that, the argument will be destructed with destructor of FArgumentSet class.
			*/
			FASTINLINE FArgumentSet& Custom(IArgument* Argument, const FName& Name, const FString& Usage = "") {
				return _Requires(Argument, Name, Usage);
			}

			FASTINLINE FArgumentSet& Custom(IArgument* Argument, TFunction<void(IArgument*)> Callback, const FName& Name, const FString& Usage = "") {
				return _Requires(Argument, Forward<TFunction<void(IArgument*)>>(Callback), Name, Usage);
			}

			template<typename Type>
			FASTINLINE FArgumentSet& Requires(Type& OutValue, const FName& Name, const FString& Usage = "") {
				static_assert(TTypedArgument<Type>::Supported, "Unsupported type specified!");
				return _Requires(new TTypedArgument<Type>(), [&](IArgument* Arg) {
					// Arguments should be copied explicitly.
					OutValue = *((const Type*)Arg->GetRaw());
				}, Name, Usage);
			}

			template<typename Type>
			FASTINLINE FArgumentSet& Requires(TFunction<void(const Type&)> Callback, const FName& Name, const FString& Usage = "") {
				static_assert(TTypedArgument<Type>::Supported, "Unsupported type specified!");
				return _Requires(new TTypedArgument<Type>(), [Callback](IArgument* Arg) {
					// Arguments should be copied explicitly.
					if (Callback) {
						Callback(*((const Type*)Arg->GetRaw()));
					}
				}, Name, Usage);
			}

		private:
			FArgumentSet& _Requires(IArgument* Argument, const FName& Name, const FString& Usage);
			FArgumentSet& _Requires(IArgument* Argument, TFunction<void(IArgument*)>&& Callback, const FName& Name, const FString& Usage);
		};
	}
}