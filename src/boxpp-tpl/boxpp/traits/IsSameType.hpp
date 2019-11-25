#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/*
		Checking given two types are same-type or not.
	*/
	template<typename A, typename B>
	struct TIsSameType : TTraitBoolConstant<false> { };

	template<typename Type>
	struct TIsSameType<Type, Type> : TTraitBoolConstant<true> { };

	/*
		Constexpr Value version of TIsSameType.
	*/
	template<typename A, typename B>
	constexpr bool IsSameType = TIsSameType<A, B>::Value;
}