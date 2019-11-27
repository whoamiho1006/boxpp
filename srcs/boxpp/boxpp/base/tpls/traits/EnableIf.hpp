#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>

namespace boxpp
{
	/* Pick type between A or B by boolean. */
	template<bool Condition, typename Type>
	struct TEnableIf;

	template<typename _Type>
	struct TEnableIf<true, _Type>
	{
		using Type = _Type;
	};

	/* Pick type between A or B by boolean. */
	template<bool Condition, typename Type>
	using EnableIf = typename TEnableIf<Condition, Type>::Type;
}