#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>

namespace boxpp
{
	/* Pick type between A or B by boolean. */
	template<bool PickA, typename A, typename B>
	struct TPickType
	{
		using Type = A;
	};

	template<typename A, typename B>
	struct TPickType<false, A, B>
	{
		using Type = B;
	};

	/* Pick type between A or B by boolean. */
	template<bool PickA, typename A, typename B>
	using PickType = typename TPickType<PickA, A, B>::Type;
}