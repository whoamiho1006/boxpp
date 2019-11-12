#ifndef __BOXPP_UTILS_FUNCTIONALITY_HPP__
#define __BOXPP_UTILS_FUNCTIONALITY_HPP__

#ifndef __BOXPP_UTILS_TYPECOMPARES_HPP__
/* Loads TypeCompares.hpp header if not loaded. */
#include <boxpp/utils/TypeCompares.hpp>
#endif

namespace boxpp
{
	namespace _ {
		template<bool PickA, typename A, typename B>
		struct PickTypeImpl
		{
			using Type = A;
		};

		template<typename A, typename B>
		struct PickTypeImpl<false, A, B>
		{
			using Type = B;
		};
	}

	/* Pick type A or B.*/
	template<bool PickA, typename A, typename B>
	using PickType = typename _::PickTypeImpl<PickA, A, B>::Type;

	namespace _ {
		template<typename Type, typename ReturnType, typename ... ArgumentTypes>
		struct IsExpectedCallableTypeImpl
		{
			struct Yes { }; struct No { };

			template<typename _Type, typename ... _ArgumentTypes>
			static PickType<
				IsSameType<QueryReturnTypeOf<_Type, _ArgumentTypes...>, ReturnType> ||
				IsConvertibleType<ReturnType, QueryReturnTypeOf<_Type, _ArgumentTypes...>>,
				Yes, No> Test(s32);

			template<typename, typename ...>
			static No Test(...);

			static constexpr bool Value = 
				IsCallableType<Type, ArgumentTypes ...> &&
				IsSameType<Yes, decltype(Test<Type, ArgumentTypes ...>(0))>;
		};
	}

	/* Determines the given type is callable and returns expected return-type or not. */
	template<typename Type, typename ReturnType, typename ... ArgumentTypes>
	constexpr bool IsExpectedCallableType = _::IsExpectedCallableTypeImpl<Type, ReturnType, ArgumentTypes ...>::Value;

	/* Determines the given type is callable and returns expected return-type or not. */
	template<typename Type, typename ReturnType, typename Left, typename Right = Left>
	constexpr bool HasExpectedOperatorInType = _::IsExpectedCallableTypeImpl<Type, ReturnType, Left, Right>::Value;

	/* Templated operator definitions. */
	namespace ops {

#define BOX_FUNCTIONALITY_OPERATOR(Name, Operation) \
		template<typename Type = void> struct Name { \
			constexpr auto operator ()(const Type& L, const Type& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}; \
		template<> struct Name<void> { template<typename Left, typename Right> \
			constexpr auto operator ()(const Left& L, const Right& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}

#define BOX_FUNCTIONALITY_OPERATOR_ASSIGN(Name, Operation) \
		template<typename Type = void> struct Name { \
			constexpr auto operator ()(Type& L, const Type& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}; \
		template<> struct Name<void> { template<typename Left, typename Right> \
			constexpr auto operator ()(Left& L, const Right& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}

#define BOX_FUNCTIONALITY_PREOPERATOR(Name, Operation) \
		template<typename Type> struct Name { \
			constexpr auto operator()(const Type& V) const \
				-> decltype(Operation V) { return Operation V; } \
		}

#define BOX_FUNCTIONALITY_POSTOPERATOR(Name, Operation) \
		template<typename Type> struct Name { \
			constexpr auto operator()(const Type& V) const \
				-> decltype(V Operation) { return V Operation; } \
		}

		BOX_FUNCTIONALITY_OPERATOR(Equal,					==);
		BOX_FUNCTIONALITY_OPERATOR(NotEqual,				!=);
		BOX_FUNCTIONALITY_OPERATOR(Greater,					>);
		BOX_FUNCTIONALITY_OPERATOR(Less,					<);
		BOX_FUNCTIONALITY_OPERATOR(GreaterEqual,			>=);
		BOX_FUNCTIONALITY_OPERATOR(LessEqual,				<=);
		BOX_FUNCTIONALITY_PREOPERATOR(PreIncrement,			++);
		BOX_FUNCTIONALITY_PREOPERATOR(PreDecrement,			--);
		BOX_FUNCTIONALITY_POSTOPERATOR(PostIncrement,		++);
		BOX_FUNCTIONALITY_POSTOPERATOR(PostDecrement,		--);
		BOX_FUNCTIONALITY_PREOPERATOR(Not,					!);
		BOX_FUNCTIONALITY_OPERATOR(And,						&&);
		BOX_FUNCTIONALITY_OPERATOR(Or,						||);
		BOX_FUNCTIONALITY_PREOPERATOR(BitwiseNot,			!);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseAnd,				&);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseOr,				|);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseXor,				&);
		BOX_FUNCTIONALITY_OPERATOR(Assign,					=);
		BOX_FUNCTIONALITY_OPERATOR(Plus,					+);
		BOX_FUNCTIONALITY_OPERATOR(Minus,					-);
		BOX_FUNCTIONALITY_OPERATOR(Multiply,				*);
		BOX_FUNCTIONALITY_OPERATOR(Modular,					%);
		BOX_FUNCTIONALITY_OPERATOR(Divide,					/);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(PlusAssign,		+=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(MinusAssign,		-=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(MultiplyAssign,	*=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(ModularAssign,	%=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(DivideAssign,		/=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(LeftShiftAssign,	<<=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(RightShiftAssign,	>>=);

#undef BOX_FUNCTIONALITY_OPERATOR
#undef BOX_FUNCTIONALITY_PREOPERATOR
#undef BOX_FUNCTIONALITY_POSTOPERATOR
	}
}

#endif // !__BOXPP_UTILS_FUNCTIONALITY_HPP__
