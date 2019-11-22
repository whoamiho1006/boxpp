#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/*
		Determines child type has derived from super or not.
	*/
	template<typename Super, typename Child>
	struct TIsDerivedType : FTraitTesterBase
	{
		static Yes Test(Super*);	// If derived.
		static No Test(...);		// Trap-back.

		static constexpr bool Value =
			IsSameType<Yes, decltype(Test((Child*)0))>;
	};

	/*
		Constexpr Value version of TIsDerivedType.
	*/
	template<typename Super, typename Child>
	constexpr bool IsDerivedType = TIsDerivedType<Super, Child>::Value;
}