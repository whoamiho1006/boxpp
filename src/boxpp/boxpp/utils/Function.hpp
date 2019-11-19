#ifndef __BOXPP_UTILS_FUNCTION_HPP__
#define __BOXPP_UTILS_FUNCTION_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/Forward.hpp>

namespace boxpp {

	template<typename FuncType>
	class TFunction;

	namespace _ {
		template<typename FuncType>
		class TFunctionBase;
		
		template<typename RetType, typename LambdaType, typename ...ArgTypes>
		using CallType = RetType(*)(void*, ArgTypes...);

		template<typename RetType, typename LambdaType, typename ...ArgTypes>
		struct TCallTypeImpl {
			FASTINLINE static CallType<RetType, LambdaType, ArgTypes ...> MakeCallType()
			{
				return [](void* InLambda, ArgTypes ... Args) {
					return (*((LambdaType*)InLambda))(Args ...);
				};
			}
		};

		template<typename LambdaType, typename ...ArgTypes>
		struct TCallTypeImpl<void, LambdaType, ArgTypes ...> {
			FASTINLINE static CallType<void, LambdaType, ArgTypes ...> MakeCallType()
			{
				return [](void* InLambda, ArgTypes ... Args) {
					(*((LambdaType*)InLambda))(Args ...);
				};
			}
		};

		template<typename RetType, typename CallbackType, typename ...ArgTypes>
		struct TCallTypeWrapperImpl {
			FASTINLINE static CallType<RetType, CallbackType, ArgTypes ...> MakeCallType()
			{
				return [](void* InLambda, ArgTypes ... Args) {
					return ((CallbackType)InLambda)(Args ...);
				};
			}
		};

		template<typename CallbackType, typename ...ArgTypes>
		struct TCallTypeWrapperImpl<void, CallbackType, ArgTypes ...> {
			FASTINLINE static CallType<void, CallbackType, ArgTypes ...> MakeCallType()
			{
				return [](void* InLambda, ArgTypes ... Args) {
					((CallbackType)InLambda)(Args ...);
				};
			}
		};

		template<typename ClassType, typename RetType, typename ... ArgTypes>
		class TFunctionByMethod
		{
		public:
			TFunctionByMethod(ClassType* Object, RetType(ClassType::* Callback)(ArgTypes ...))
				: Object(Object), Callback(Callback)
			{
			}

		public:
			ClassType* Object;
			RetType(ClassType::* Callback)(ArgTypes ...);
		};

		template<typename ClassType, typename RetType, typename ... ArgTypes>
		class TFunctionByMethodConst
		{
		public:
			TFunctionByMethodConst(const ClassType* Object, RetType(ClassType::* Callback)(ArgTypes ...) const)
				: Object(Object), Callback(Callback)
			{
			}

		public:
			const ClassType* Object;
			RetType(ClassType::* Callback)(ArgTypes ...) const;
		};


		template<typename RetType, typename ClassType, typename ...ArgTypes>
		struct TMethodTypeWrapperImpl {
			FASTINLINE static CallType<RetType, ClassType, ArgTypes ...> MakeCallType()
			{
				using WrapperType = TFunctionByMethod<ClassType, RetType, ArgTypes ...>;
				return [](void* InLambda, ArgTypes ... Args) {
					WrapperType* Wrapper = (WrapperType*)InLambda;

					RetType(ClassType::* Callback)(ArgTypes ...) = Wrapper->Callback;
					ClassType* Object = Wrapper->Object;

					return (Object->*Callback)(Forward<ArgTypes>(Args) ...);
				};
			}
		};

		template<typename ClassType, typename ...ArgTypes>
		struct TMethodTypeWrapperImpl<void, ClassType, ArgTypes ...> {
			FASTINLINE static CallType<void, ClassType, ArgTypes ...> MakeCallType()
			{
				using WrapperType = TFunctionByMethod<ClassType, void, ArgTypes ...>;
				return [](void* InLambda, ArgTypes ... Args) {
					WrapperType* Wrapper = (WrapperType*)InLambda;

					void(ClassType::* Callback)(ArgTypes ...) = Wrapper->Callback;
					ClassType* Object = Wrapper->Object;

					(Object->*Callback)(Forward<ArgTypes>(Args) ...);
				};
			}
		};

		template<typename RetType, typename ClassType, typename ...ArgTypes>
		struct TMethodConstTypeWrapperImpl {
			FASTINLINE static CallType<RetType, ClassType, ArgTypes ...> MakeCallType()
			{
				using WrapperType = TFunctionByMethodConst<ClassType, RetType, ArgTypes ...>;
				return [](void* InLambda, ArgTypes ... Args) {
					WrapperType* Wrapper = (WrapperType*)InLambda;

					RetType(ClassType::* Callback)(ArgTypes ...) const = Wrapper->Callback;
					const ClassType* Object = Wrapper->Object;

					return (Object->*Callback)(Forward<ArgTypes>(Args) ...);
				};
			}
		};

		template<typename ClassType, typename ...ArgTypes>
		struct TMethodConstTypeWrapperImpl<void, ClassType, ArgTypes ...> {
			FASTINLINE static CallType<void, ClassType, ArgTypes ...> MakeCallType()
			{
				using WrapperType = TFunctionByMethodConst<ClassType, void, ArgTypes ...>;
				return [](void* InLambda, ArgTypes ... Args) {
					WrapperType* Wrapper = (WrapperType*)InLambda;

					void(ClassType::* Callback)(ArgTypes ...) const = Wrapper->Callback;
					const ClassType* Object = Wrapper->Object;

					(Object->*Callback)(Forward<ArgTypes>(Args) ...);
				};
			}
		};

		template<typename RetType, typename ... ArgTypes>
		class TFunctionBase<RetType(ArgTypes ...)>
		{
		public:
			typedef TFunctionBase<RetType(ArgTypes ...)> BaseType;
			typedef RetType(*CallType)(void* InLambda, ArgTypes ...);

		public:
			FASTINLINE TFunctionBase()
				: Lambda(nullptr), Clone(nullptr), Cleanup(nullptr), Call(nullptr)
			{
			}

			FASTINLINE TFunctionBase(const BaseType& Other)
				: Lambda(nullptr), Clone(Other.Clone), Cleanup(Other.Cleanup), Call(Other.Call)
			{
				if (Other.Lambda)
				{
					Lambda = Clone(Other.Lambda);
				}
			}

			FASTINLINE TFunctionBase(BaseType&& Other)
				: Lambda(Other.Lambda), Clone(Other.Clone), Cleanup(Other.Cleanup), Call(Other.Call)
			{
				Other.Lambda = nullptr;
				Other.Clone = nullptr;
				Other.Cleanup = nullptr;
			}

			FASTINLINE TFunctionBase(RetType(*Callback)(ArgTypes ...))
			{
				Lambda = (void*)Callback;
				Clone = [](void* InLambda) { return InLambda; };
				Cleanup = nullptr;
				Call = TCallTypeWrapperImpl<RetType, RetType(*)(ArgTypes ...), ArgTypes ...>::MakeCallType();
			}

			template<typename ClassType>
			FASTINLINE TFunctionBase(ClassType* Object, RetType(ClassType::* Callback)(ArgTypes ...))
			{
				using WrapperType = TFunctionByMethod<ClassType, RetType, ArgTypes ...>;

				Lambda = new WrapperType(Object, Callback);
				Clone = [](void* InLambda) {
					WrapperType* Wrapper = (WrapperType*)InLambda;
					return (void*) new WrapperType(Wrapper->Object, Wrapper->Callback);
				};

				Cleanup = [](void* InLambda) { delete ((WrapperType*)InLambda); };
				Call = TMethodTypeWrapperImpl<RetType, ClassType, ArgTypes ...>::MakeCallType();
			}

			template<typename ClassType>
			FASTINLINE TFunctionBase(const ClassType* Object, RetType(ClassType::* Callback)(ArgTypes ...) const)
			{
				using WrapperType = TFunctionByMethodConst<ClassType, RetType, ArgTypes ...>;

				Lambda = new WrapperType(Object, Callback);
				Clone = [](void* InLambda) {
					WrapperType* Wrapper = (WrapperType*)InLambda;
					return (void*) new WrapperType(Wrapper->Object, Wrapper->Callback);
				};

				Cleanup = [](void* InLambda) { delete ((WrapperType*)InLambda); };
				Call = TMethodConstTypeWrapperImpl<RetType, ClassType, ArgTypes ...>::MakeCallType();
			}

			template<typename LambdaType>
			FASTINLINE TFunctionBase(const LambdaType& Lambda)
				: Lambda(new LambdaType(Lambda))
			{
				Clone = [](void* InLambda) {
					return (void*)new LambdaType(*((const LambdaType*)InLambda));
				};

				Cleanup = [](void* InLambda) {
					delete ((LambdaType*)InLambda);
				};

				Call = TCallTypeImpl<RetType, LambdaType, ArgTypes ...>::MakeCallType();
			}

			template<typename LambdaType>
			FASTINLINE TFunctionBase(LambdaType&& Lambda)
				: Lambda(new LambdaType(Forward<LambdaType>(Lambda)))
			{
				Clone = [](void* InLambda) {
					return (void*)new LambdaType(*((const LambdaType*)InLambda));
				};

				Cleanup = [](void* InLambda) {
					delete ((LambdaType*)InLambda);
				};

				Call = TCallTypeImpl<RetType, LambdaType, ArgTypes ...>::MakeCallType();
			}

			FASTINLINE ~TFunctionBase()
			{
				if (Lambda && Cleanup) {
					Cleanup(Lambda);
				}

				Lambda = nullptr;
				Clone = nullptr;
				Cleanup = nullptr;
				Call = nullptr;
			}

		protected:
			void* Lambda;
			void*(*Clone)(void*);
			void(*Cleanup)(void*);

		protected:
			CallType Call;
		};
	}

	template<typename RetType, typename ... ArgTypes>
	class TFunction<RetType(ArgTypes ...)>
		: public _::TFunctionBase<RetType(ArgTypes ...)>
	{
	private:
		typedef _::TFunctionBase<RetType(ArgTypes ...)> BaseType;

	public:
		typedef TFunction<RetType(ArgTypes ...)> SelfType;

	public:
		FASTINLINE TFunction(nullptr_t = nullptr) : BaseType(nullptr) { }

		template<typename LambdaType>
		FASTINLINE TFunction(const LambdaType& Lambda) : BaseType(Lambda) { }

		template<typename LambdaType>
		FASTINLINE TFunction(LambdaType&& Lambda) : BaseType(Forward<LambdaType>(Lambda)) { }

		FASTINLINE TFunction(const SelfType& Other) : BaseType(Other) { }
		FASTINLINE TFunction(SelfType&& Other) : BaseType(Forward<SelfType>(Other)) { }

		template<typename ClassType>
		FASTINLINE TFunction(ClassType* Object, RetType(ClassType::* Callback)(ArgTypes...))
			: BaseType(Object, Callback) { }

		template<typename ClassType>
		FASTINLINE TFunction(const ClassType* Object, RetType(ClassType::* Callback)(ArgTypes...) const)
			: BaseType(Object, Callback) { }

		FASTINLINE ~TFunction() { }
		
	public:
		FASTINLINE operator bool() const { return this->Lambda && this->Call; }
		FASTINLINE bool operator !() const { return this->Lambda && this->Call; }

	public:
		FASTINLINE RetType operator()(ArgTypes ... Args) const {
			return this->Call(this->Lambda, Forward<ArgTypes>(Args) ...);
		}

	public:
		FASTINLINE SelfType& operator =(const SelfType& Other)
		{
			if (Other)
			{
				this->Clone = Other.Clone;
				this->Cleanup = Other.Cleanup;
				this->Call = Other.Call;

				this->Lambda = this->Clone(Other.Lambda);
			}

			else
			{
				if (this->Lambda && this->Cleanup) {
					this->Cleanup(this->Lambda);
				}

				this->Lambda = nullptr;
				this->Clone = nullptr;
				this->Cleanup = nullptr;
				this->Call = nullptr;
			}

			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other)
		{
			Swap(this->Lambda, Other.Lambda);
			Swap(this->Clone, Other.Clone);
			Swap(this->Cleanup, Other.Cleanup);
			Swap(this->Call, Other.Call);

			return *this;
		}
	};

	template<typename ... ArgTypes>
	class TFunction<void(ArgTypes ...)>
		: public _::TFunctionBase<void(ArgTypes ...)>
	{
	private:
		typedef _::TFunctionBase<void(ArgTypes ...)> BaseType;

	public:
		typedef TFunction<void(ArgTypes ...)> SelfType;

	public:
		FASTINLINE TFunction(nullptr_t = nullptr) : BaseType(nullptr) { }

		template<typename LambdaType>
		FASTINLINE TFunction(const LambdaType& Lambda) : BaseType(Lambda) { }

		template<typename LambdaType>
		FASTINLINE TFunction(LambdaType&& Lambda) : BaseType(Forward<LambdaType>(Lambda)) { }

		FASTINLINE TFunction(const SelfType& Other) : BaseType(Other) { }
		FASTINLINE TFunction(SelfType&& Other) : BaseType(Forward<SelfType>(Other)) { }

		template<typename ClassType>
		FASTINLINE TFunction(ClassType* Object, void(ClassType::* Callback)(ArgTypes...))
			: BaseType(Object, Callback) { }

		template<typename ClassType>
		FASTINLINE TFunction(const ClassType* Object, void(ClassType::* Callback)(ArgTypes...) const)
			: BaseType(Object, Callback) { }

		FASTINLINE ~TFunction() { }

	public:
		FASTINLINE operator bool() const { return this->Lambda && this->Call; }
		FASTINLINE bool operator !() const { return this->Lambda && this->Call; }

	public:
		FASTINLINE void operator()(ArgTypes ... Args) const {
			this->Call(this->Lambda, Forward<ArgTypes>(Args) ...);
		}

	public:
		FASTINLINE SelfType& operator =(const SelfType& Other)
		{
			if (Other)
			{
				this->Clone = Other.Clone;
				this->Cleanup = Other.Cleanup;
				this->Call = Other.Call;

				this->Lambda = this->Clone(Other.Lambda);
			}

			else
			{
				if (this->Lambda && this->Cleanup) {
					this->Cleanup(this->Lambda);
				}

				this->Lambda = nullptr;
				this->Clone = nullptr;
				this->Cleanup = nullptr;
				this->Call = nullptr;
			}

			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other)
		{
			Swap(this->Lambda, Other.Lambda);
			Swap(this->Clone, Other.Clone);
			Swap(this->Cleanup, Other.Cleanup);
			Swap(this->Call, Other.Call);

			return *this;
		}
	};

}

#endif // !__BOXPP_UTILS_FUNCTION_HPP__