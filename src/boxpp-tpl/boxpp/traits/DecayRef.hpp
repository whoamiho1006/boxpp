#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Decay reference to non-reference. */
	template<typename _Type>
	struct TDecayRef { using Type = _Type; };

	template<typename _Type>
	struct TDecayRef<_Type&>{ using Type = _Type; };

	template<typename _Type>
	struct TDecayRef<_Type&&> { using Type = _Type; };

	/* Decay reference to non-reference. */
	template<typename _Type>
	using DecayRef = typename TDecayRef<_Type>::Type;
}