#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Decay const-type to non const-type. */
	template<typename _Type>
	struct TDecayConst { using Type = _Type; };

	template<typename _Type>
	struct TDecayConst<const _Type> { using Type = _Type; };

	template<typename _Type>
	struct TDecayConst<const volatile _Type> { using Type = volatile _Type; };

	/* Decay const-type to non const-type. */
	template<typename _Type>
	using DecayConst = typename TDecayConst<_Type>::Type;
}