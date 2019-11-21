#pragma once
#include <boxpp/BaseTraits.hpp>
#include <boxpp/traits/IsSameType.hpp>
#include <boxpp/traits/IsConstructibleType.hpp>

namespace boxpp
{
	/* Determines Type can be callable or not. */
	template<typename Type, typename ... ArgumentTypes>
	struct TIsCallableType : FTraitTesterBase
	{
		template<typename RetType = decltype((DeclVal<Type>())(DeclVal<ArgumentTypes>() ...))>
		static RetType Test(s32);
		static No Test(...);

		static constexpr bool Value = !IsSameType<No, decltype(Test(0))>;
	};

	/* Queries the return type of specified callable type. */
	template<typename _Type, typename ... ArgumentTypes>
	struct TQueryReturnType { using Type = decltype((DeclVal<_Type>())(DeclVal<ArgumentTypes>() ...)); };

	/* Queries the return type of specified callable type. */
	template<typename Type, typename ... ArgumentTypes>
	using QueryReturnType = typename TQueryReturnType<Type, ArgumentTypes...>::Type;

	/* Determines Type can be callable or not. */
	template<typename Type, typename ... ArgumentTypes>
	constexpr bool IsCallableType = TIsCallableType<Type, ArgumentTypes ...>::Value;

	/* Determines the callable type is expected one or not. */
	template<typename CallableType, typename ReturnType, typename ... ArgumentTypes>
	struct TIsExpectedCallableType : FTraitTesterBase
	{
		template<typename, typename ...>
		static No Test(...);

		template<typename _Type, typename ... _ArgumentTypes>
		static PickType<
			IsSameType<QueryReturnTypeOf<_Type, _ArgumentTypes...>, ReturnType> ||
			IsConvertibleType<ReturnType, QueryReturnTypeOf<_Type, _ArgumentTypes...>>,
			Yes, No> Test(s32);

		static constexpr bool Value =
			IsCallableType<CallableType, ArgumentTypes ...> &&
			IsSameType<Yes, decltype(Test<Type, ArgumentTypes ...>(0))>;
	};

	/* Determines the callable type is expected one or not. */
	template<typename Type, typename ReturnType, typename ... ArgumentTypes>
	constexpr bool IsExpectedCallableType = TIsExpectedCallableType<Type, ReturnType, ArgumentTypes ...>::Value;

}