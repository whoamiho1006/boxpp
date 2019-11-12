#ifndef __BOXPP_UTILS_TYPECOMPARES_HPP__
#define __BOXPP_UTILS_TYPECOMPARES_HPP__

#ifndef __BOXPP_UTILS_FORWARD_HPP__
/* Loads Forward.hpp header if not loaded. */
#include <boxpp/utils/Forward.hpp>
#endif

namespace boxpp
{
	/* Makes decl-value from given type. (not for real-use) */
	template<typename Type>
	constexpr Type DeclVal();

	/* Determines the given type is plain old data or not. */
	template<typename Type>
	constexpr bool IsTypePOD = __is_pod(Type);

	namespace _ {
		template<typename A, typename B>
		struct IsSameTypeImpl
		{
			static constexpr bool Value = false;
		};

		template<typename Something>
		struct IsSameTypeImpl<Something, Something>
		{
			static constexpr bool Value = true;
		};
	}

	/* Determines given two types are same or not. */
	template<typename A, typename B>
	constexpr bool IsSameType = _::IsSameTypeImpl<A, B>::Value;

	namespace _ {
		template<typename Super, typename Child>
		struct IsDerivedTypeImpl
		{
			struct Yes { }; struct No { };

			static Yes Test(Super*);	// If derived.
			static No Test(...);		// Trap-back.

			static constexpr bool Value =
				IsSameType<Yes, decltype(Test((Child*)0))>;
		};
	}

	/* Determines given child type is derived from super type or not. */
	template<typename Super, typename Child>
	constexpr bool IsDerivedType = _::IsDerivedTypeImpl<Super, Child>::Value;

	/* Determines given src-type can be convertible to dest-type or not. */
	template<typename DestType, typename SrcType>
	constexpr bool IsConvertibleType = __is_convertible_to(SrcType, DestType);

	namespace _ {
		template<typename Type, typename ... ArgumentTypes>
		struct IsCallableTypeImpl
		{
			struct No { };

			template<typename RetType = decltype((DeclVal<Type>())(DeclVal<ArgumentTypes>() ...))>
			static RetType Test(s32);
			static No Test(...);

			static constexpr bool Value = !IsSameType<No, decltype(Test(0))>;
		};

		template<typename _Type, typename ... ArgumentTypes>
		struct QueryReturnTypeImpl
		{
			using Type = decltype((DeclVal<_Type>())(DeclVal<ArgumentTypes>() ...));
		};
	}

	/* Determines the given type is callable or not. */
	template<typename Type, typename ... ArgumentTypes>
	constexpr bool IsCallableType = _::IsCallableTypeImpl<Type, ArgumentTypes ...>::Value;

	/* Queries the return type of specified callable type. */
	template<typename Type, typename ... ArgumentTypes>
	using QueryReturnTypeOf = typename _::QueryReturnTypeImpl<Type, ArgumentTypes...>::Type;
}

#endif // !__BOXPP_UTILS_TYPECOMPARES_HPP__
