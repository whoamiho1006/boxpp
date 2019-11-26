#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/BypassType.hpp>

namespace boxpp {

	template<typename FuncType>
	class TCallback;

	template<typename RetType, typename ... ArgTypes>
	class TCallbackStorage
	{
	public:
		typedef RetType(*CallableType)(ArgTypes ...);

	public:
		TCallbackStorage(CallableType Callable) 
			: Callable(Callable)
		{
		}

	protected:
		CallableType Callable;

	public:
		FASTINLINE RetType operator ()(BypassType<ArgTypes> ... Args) const {
			return Callable(Args ...);
		}
	};

	template<typename ... ArgTypes>
	class TCallbackStorage<void, ArgTypes ...>
	{
	public:
		typedef void(*CallableType)(ArgTypes ...);

	public:
		TCallbackStorage(CallableType Callable)
			: Callable(Callable)
		{
		}

	public:
		FASTINLINE void operator ()(BypassType<ArgTypes> ... Args) const {
			Callable(Args ...);
		}
	};

	template<typename RetType, typename ... ArgTypes>
	class TCallback<RetType(ArgTypes ...)>
		: public TCallbackStorage<RetType, ArgTypes ...>
	{
	public:
		using CallbackStorage = TCallbackStorage<RetType, ArgTypes ...>;
		typedef TCallback<RetType(ArgTypes ...)> SelfType;

	public:
		TCallback()
			: CallbackStorage(nullptr)
		{
		}

		TCallback(CallableType Callable)
			: CallbackStorage(Callable)
		{
		}

		TCallback(const SelfType& Other)
			: CallbackStorage(Other.Callable)
		{
		}

		TCallback(SelfType&& Other)
			: CallbackStorage(Other.Callable)
		{
			Other.Callable = nullptr;
		}

		~TCallback()
		{
		}

	public:
		FASTINLINE operator bool() const { return this->Callable; }
		FASTINLINE bool operator !() const { return !this->Callable; }
		FASTINLINE CallableType operator *() const { return this->Callable; }

	public:
		FASTINLINE bool operator ==(const SelfType& Other) const { return this->Callable == Other.Callable; }
		FASTINLINE bool operator !=(const SelfType& Other) const { return this->Callable != Other.Callable; }

		FASTINLINE bool operator ==(CallableType Other) const { return this->Callable == Other; }
		FASTINLINE bool operator !=(CallableType Other) const { return this->Callable != Other; }

	public:
		FASTINLINE SelfType& operator =(CallableType Callable) {
			this->Callable = Callable;
			return *this;
		}

		FASTINLINE SelfType& operator =(const SelfType& Other) {
			this->Callable = Other.Callable;
			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other) {
			if (this != &Other) {
				CallableType Callable = this->Callable;
				this->Callable = Other.Callable;
				Other.Callable = Callable;
			}

			return *this;
		}
	};

}