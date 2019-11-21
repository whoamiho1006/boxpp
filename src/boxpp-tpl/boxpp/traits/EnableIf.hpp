#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Pick type between A or B by boolean. */
	template<bool Condition, typename CallableType>
	struct TEnableIf;

	template<typename _Type>
	struct TEnableIf<true, _Type>
	{
		using Type = _Type;
	};

	/* Pick type between A or B by boolean. */
	template<bool Condition, typename CallableType>
	using EnableIf = typename TEnableIf<Condition, CallableType>::Type;
}