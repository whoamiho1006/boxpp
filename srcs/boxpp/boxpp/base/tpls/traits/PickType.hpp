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

	template<bool PickA, typename A, typename B>
	struct TPickConstant
	{ static constexpr A Pick(A ValueA, B ValueB) { return ValueA; } };

	template<typename A, typename B>
	struct TPickConstant<false, A, B>
	{
		static constexpr B Pick(A ValueA, B ValueB) { return ValueB; }
	};
}