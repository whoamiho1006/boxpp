#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/BypassType.hpp>

namespace boxpp
{
	template<typename FuncType>
	class TMethod;

	template<typename RetType, typename ... ArgTypes>
	class TMethodStorage;

	template<typename RetType, typename ... ArgTypes>
	class TMethodInvoke
	{
	public:
		typedef TMethodStorage<RetType, ArgTypes ...> StorageType;
		typedef RetType(*CallableType)(StorageType*, void*, BypassType<ArgTypes> ...);

	public:
		TMethodInvoke() 
			: Callable(nullptr)
		{
		}

	protected:
		CallableType Callable;

	public:
		template<typename ContextType>
		FASTINLINE RetType operator ()(ContextType* Context, BypassType<ArgTypes> ... Args) const
		{
			return Callable((StorageType*)this, Context, Args ...);
		}
	};

	template<typename ... ArgTypes>
	class TMethodInvoke<void, ArgTypes ...>
	{
	public:
		typedef TMethodStorage<void, ArgTypes ...> StorageType;
		typedef void(*CallableType)(StorageType*, void*, BypassType<ArgTypes> ...);

	public:
		TMethodInvoke()
			: Callable(nullptr)
		{
		}

	protected:
		CallableType Callable;

	public:
		template<typename ContextType>
		FASTINLINE void operator ()(ContextType* Context, BypassType<ArgTypes> ... Args) const
		{
			Callable((StorageType*)this, Context, Args ...);
		}
	};

	template<typename RetType, typename ... ArgTypes>
	class TMethodStorage 
		: public TMethodInvoke<RetType, ArgTypes ...>
	{
	public:
		typedef TMethodStorage<RetType, ArgTypes ...> StorageType;
		typedef void(*CallableType)(StorageType*, void*, BypassType<ArgTypes> ...);
		template<typename, typename ...> friend class TMethodStorage;

	public:
		TMethodStorage(nullptr_t = nullptr)
			: StoredMethod(nullptr)
		{
		}

		template<typename ClassType>
		TMethodStorage(RetType(ClassType::* Callable)(ArgTypes ...))
		{
			using MethodType = RetType(ClassType::*)(ArgTypes ...);
			union ConvType {
				MethodType Callable;
				void* Void;
			} Converter;

			Converter.Callable = Callable;
			StoredMethod = Converter.Void;

			this->Callable = [](StorageType* Storage, void* Binding, BypassType<ArgTypes> ... Args) {
				ClassType* BoundObject = (ClassType*)Binding;
				ConvType Conv;

				Conv.Void = Storage->StoredMethod;
				MethodType Method = Conv.Callable;

				return (BoundObject->*Method)(Args ...);
			};
		}

		template<typename ClassType>
		TMethodStorage(RetType(ClassType::* Callable)(ArgTypes ...) const)
		{
			using MethodType = RetType(ClassType::*)(ArgTypes ...) const;
			union ConvType {
				MethodType Callable;
				void* Void;
			} Converter;

			Converter.Callable = Callable;
			StoredMethod = Converter.Void;

			this->Callable = [](StorageType* Storage, void* Binding, BypassType<ArgTypes> ... Args) {
				ClassType* BoundObject = (ClassType*)Binding;
				ConvType Conv;

				Conv.Void = Storage->StoredMethod;
				MethodType Method = Conv.Callable;

				return (BoundObject->*Method)(Args ...);
			};
		}

	protected:
		void* StoredMethod;
	};

	template<typename RetType, typename ... ArgTypes>
	class TMethod<RetType(ArgTypes ...)>
		: public TMethodStorage<RetType, ArgTypes ...>
	{
	public:
		using MethodStorage = TMethodStorage<RetType, ArgTypes ...>;
		typedef TMethod<RetType(ArgTypes ...)> SelfType;

	public:
		TMethod()
			: MethodStorage(nullptr)
		{
		}

		template<typename ClassType>
		TMethod(RetType(ClassType::* Callable)(ArgTypes ...))
			: MethodStorage(Callable)
		{
		}

		template<typename ClassType>
		TMethod(RetType(ClassType::* Callable)(ArgTypes ...) const)
			: MethodStorage(Callable)
		{
		}

		TMethod(const SelfType& Other)
		{
			this->Callable = Other.Callable;
			this->StoredMethod = Other.StoredMethod;
		}

		TMethod(SelfType&& Other)
		{
			this->Callable = Other.Callable;
			this->StoredMethod = Other.StoredMethod;

			Other.Callable = nullptr;
			Other.StoredMethod = nullptr;
		}

		~TMethod()
		{
		}

	public:
		FASTINLINE operator bool() const { return this->Callable; }
		FASTINLINE bool operator !() const { return !this->Callable; }

	public:
		FASTINLINE bool operator ==(const SelfType& Other) const { return this->StoredMethod == Other.StoredMethod; }
		FASTINLINE bool operator !=(const SelfType& Other) const { return this->StoredMethod != Other.StoredMethod; }

	public:
		template<typename ClassType>
		FASTINLINE SelfType& operator =(RetType(ClassType::* Callable)(ArgTypes ...)) {
			TMethod<RetType(ArgTypes ...)> Method(Callable);

			this->Callable = Method.Callable;
			this->StoredMethod = Method.StoredMethod;

			return *this;
		}

		FASTINLINE SelfType& operator =(const SelfType& Other) {
			this->Callable = Other.Callable;
			this->StoredMethod = Other.StoredMethod;
			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other) {
			if (this != &Other) {
				Swap(this->Callable, Other.Callable);
				Swap(this->StoredMethod, Other.StoredMethod);
			}

			return *this;
		}

	};
}