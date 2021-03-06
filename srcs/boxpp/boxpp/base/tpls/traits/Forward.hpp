#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>
#include <boxpp/base/tpls/traits/DecayRef.hpp>

namespace boxpp
{
	/* Cast given LValue reference to be RValue reference. */
	template<typename Type>
	constexpr Type&& Forward(DecayRef<Type>& Ref) noexcept {
		return static_cast<Type&&>(Ref);
	}

	template<typename Type>
	constexpr Type&& Forward(DecayRef<Type>&& Ref) noexcept {
		//static_assert(!IsLeftReferenceType<Type>, "Error: boxpp::Forward(REF) method requires LValue reference!");

		return static_cast<Type&&>(Ref);
	}
}