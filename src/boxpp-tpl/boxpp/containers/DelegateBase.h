#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/Forward.hpp>
#include <boxpp/SharedPtr.hpp>

namespace boxpp
{
	namespace _ {
		template<typename T>
		struct TDelegateBypassType { using Type = const T&; };

		template<typename T>
		struct TDelegateBypassType<T&> { using Type = T&; };

		template<typename T>
		struct TDelegateBypassType<const T&> { using Type = const T&; };

		template<typename T>
		struct TDelegateBypassType<volatile T&> { using Type = volatile T&; };

		template<typename T>
		struct TDelegateBypassType<const volatile T&> { using Type = const volatile T&; };

		template<typename T>
		struct TDelegateBypassType<T&&> { using Type = T&&; };

		template<typename T>
		using DelegateBypassType = typename TDelegateBypassType<T>::Type;
	}

	template<typename ... ArgTypes>
	class TDelegateBase
	{
	public:
		virtual ~TDelegateBase() { }

		enum class ECallableType
		{
			Lambda,
			Callback,
			Method,
			Shared
		};

		class ICallable
		{
		public:
			virtual ~ICallable() { }

		public:
			virtual ECallableType GetType() const { return ECallableType::Callback; }
			virtual void Invoke(_::DelegateBypassType<ArgTypes> ... Args) = 0;
		};

		struct FHandle { void* Callable; };

		template<typename LambdaType>
		class TLambdaCallable : LambdaType, public ICallable
		{
		public:
			TLambdaCallable(LambdaType&& Lambda)
				: LambdaType(Forward<LambdaType>(Lambda))
			{
			}

		public:
			virtual ECallableType GetType() const { return ECallableType::Lambda; }
			virtual void Invoke(_::DelegateBypassType<ArgTypes> ... Args) override {
				(*static_cast<LambdaType*>(this))(Args ...);
			}
		};

		class FCallbackCallable : public ICallable
		{
		public:
			FCallbackCallable(void (* Callback)(ArgTypes ... Args))
				: Callback(Callback)
			{
			}

		public:
			virtual ECallableType GetType() const { return ECallableType::Callback; }
			virtual void Invoke(_::DelegateBypassType<ArgTypes> ... Args) override {
				Callback(Args ...);
			}

		private:
			void(*Callback)(ArgTypes ... Args);
		};

		template<typename ClassType>
		class TClassCallable : public ICallable
		{
		public:
			TClassCallable(ClassType* Object, void (ClassType::* Method)(ArgTypes...)) 
				: Object(Object), Method(Method), ConstMethod(nullptr)
			{
			}

			TClassCallable(const ClassType* Object, void (ClassType::* Method)(ArgTypes...) const)
				: Object(const_cast<ClassType*>(Object)), Method(nullptr), ConstMethod(Method)
			{
			}

		public:
			virtual ECallableType GetType() const { return ECallableType::Method; }
			virtual void Invoke(_::DelegateBypassType<ArgTypes> ... Args) override {
				if (Object) {
					if (Method) {
						(Object->*Method)(Args ...);
					}

					else if (ConstMethod) {
						(Object->*ConstMethod)(Args ...);
					}
				}
			}

		private:
			ClassType* Object;
			void (ClassType::* Method)(ArgTypes... Args);
			void (ClassType::* ConstMethod)(ArgTypes... Args) const;
		};

		template<typename ClassType>
		class TSharedCallable : public ICallable
		{
		public:
			TSharedCallable(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...))
				: Object(Object), Method(Method), ConstMethod(nullptr)
			{
			}

			TSharedCallable(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...) const)
				: Object(Object), Method(nullptr), ConstMethod(Method)
			{
			}

		public:
			virtual ECallableType GetType() const { return ECallableType::Shared; }
			virtual void Invoke(_::DelegateBypassType<ArgTypes> ... Args) override {
				if (Object) {
					ClassType* Deref = Object.GetRaw();

					if (Method) {
						(Deref->*Method)(Args ...);
					}

					else if (ConstMethod) {
						(Deref->*ConstMethod)(Args ...);
					}
				}
			}

		private:
			TWeakPtr<ClassType> Object;
			void (ClassType::* Method)(ArgTypes... Args);
			void (ClassType::* ConstMethod)(ArgTypes... Args) const;
		};

		template<typename LambdaType>
		FASTINLINE static ICallable* MakeCallableLambda(LambdaType&& Lambda) {
			return new TLambdaCallable<LambdaType>(Lambda);
		}

		FASTINLINE static ICallable* MakeCallableCallback(void(*Callback)(ArgTypes ... Args)) {
			return new FCallbackCallable(Callback);
		}

		template<typename ClassType>
		FASTINLINE static ICallable* MakeCallableMethod(ClassType* Object, void (ClassType::* Method)(ArgTypes...)) {
			return new TClassCallable<ClassType>(Object, Method);
		}

		template<typename ClassType>
		FASTINLINE static ICallable* MakeCallableMethod(const ClassType* Object, void (ClassType::* Method)(ArgTypes...) const) {
			return new TClassCallable<ClassType>(Object, Method);
		}

		template<typename ClassType>
		FASTINLINE static ICallable* MakeCallableShared(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...)) {
			return new TSharedCallable<ClassType>(Object, Method);
		}

		template<typename ClassType>
		FASTINLINE static ICallable* MakeCallableShared(const TSharedPtr<ClassType>& Object, void (ClassType::* Method)(ArgTypes...) const) {
			return new TSharedCallable<ClassType>(Object, Method);
		}
	};

}