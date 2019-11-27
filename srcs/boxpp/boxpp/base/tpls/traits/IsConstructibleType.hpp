#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>
#include <boxpp/base/tpls/traits/IsPodType.hpp>

namespace boxpp
{
	/* Determines given type can be constructible by given arguments. */
	template<typename Type, typename ... ArgTypes>
	struct TIsConstructibleType : TTraitBoolConstant<__is_constructible(Type, ArgTypes ...)> { };

	/* Constexpr Value version of TIsConstructible. */
	template<typename Type, typename ... ArgTypes>
	constexpr bool IsConstructibleType = TIsConstructibleType<Type, ArgTypes ...>::Value;

	/* Determines given type is move-constructible or not. */
	template<typename Type>
	struct TIsMoveConstructibleType : TTraitBoolConstant<
		__is_constructible(Type, Type&&) || IsPodType<Type>> { };

	/* Determines given type is copy-constructible or not. */
	template<typename Type>
	struct TIsCopyConstructibleType : TTraitBoolConstant<
		__is_constructible(Type, const Type&) || IsPodType<Type>> { };

	/* Constexpr Value version of TIsMoveConstructible. */
	template<typename Type>
	constexpr bool IsMoveConstructibleType = TIsMoveConstructibleType<Type>::Value;

	/* Constexpr Value version of TIsCopyConstructible. */
	template<typename Type>
	constexpr bool IsCopyConstructibleType = TIsCopyConstructibleType<Type>::Value;
}