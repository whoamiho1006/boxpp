#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Determines the given type is POD or not. */
	template<typename CallableType>
	struct TIsPodType : TTraitBoolConstant<__is_pod(CallableType)> { };

	/* Determines the given type is POD or not. */
	template<typename CallableType>
	constexpr bool IsPodType = TIsPodType<CallableType>::Value;
}