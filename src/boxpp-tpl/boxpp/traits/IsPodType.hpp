#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Determines the given type is POD or not. */
	template<typename Type>
	struct TIsPodType : TTraitBoolConstant<__is_pod(Type)> { };

	/* Determines the given type is POD or not. */
	template<typename Type>
	constexpr bool IsPodType = TIsPodType<Type>::Value;
}