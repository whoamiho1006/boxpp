#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/containers/DelegateBase.h>

namespace boxpp
{
	template<typename ... ArgTypes>
	class TDelegate : public TDelegateBase<ArgTypes ...>
	{
	public:
		using BaseType = TDelegateBase<ArgTypes ...>;
		using SelfType = TDelegate<ArgTypes ...>;

	public:
		TDelegate()
			: Callable(nullptr)
		{

		}

		TDelegate(const SelfType&) = delete;
		TDelegate(SelfType&&) = delete;

		~TDelegate()
		{
			Unbind();
		}

	private:
		BaseType::ICallable* Callable;

	public:
		FASTINLINE operator bool() const { return Callable; }
		FASTINLINE bool operator !() const { return !Callable; }

	public:
		template<typename LambdaType>
		FASTINLINE bool BindLambda(LambdaType&& Lambda) {
			if (!Callable) {
				Callable = BaseType::MakeCallableLambda(Forward<LambdaType>(Lambda));
				return true;
			}

			return false;
		}

		FASTINLINE bool BindCallback(void(*Callback)(ArgTypes ... Args)) {
			if (!Callable) {
				Callable = BaseType::MakeCallableCallback(Callback);
				return true;
			}

			return false;
		}

		template<typename ClassType>
		FASTINLINE bool BindMethod(ClassType* Object, void (ClassType::* Method)(ArgTypes...)) {
			if (!Callable) {
				Callable = BaseType::MakeCallableMethod(Object, Method);
				return true;
			}

			return false;
		}

		template<typename ClassType>
		FASTINLINE bool BindMethod(ClassType* Object, void (ClassType::* Method)(ArgTypes...) const) {
			if (!Callable) {
				Callable = BaseType::MakeCallableMethod(Object, Method);
				return true;
			}

			return false;
		}

		template<typename ClassType>
		FASTINLINE bool BindShared(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...)) {
			if (!Callable) {
				Callable = BaseType::MakeCallableShared(Object, Method);
				return true;
			}

			return false;
		}

		template<typename ClassType>
		FASTINLINE bool BindShared(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...) const) {
			if (!Callable) {
				Callable = BaseType::MakeCallableShared(Object, Method);
				return true;
			}

			return false;
		}

		FASTINLINE void Unbind() {
			if (Callable) {
				delete Callable;
				Callable = nullptr;
			}
		}

		FASTINLINE void Invoke(_::DelegateBypassType<ArgTypes> ... Args) {
			if (Callable) {
				Callable->Invoke(Args ...);
			}
		}
	};

}