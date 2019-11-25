#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	template<typename _IntegralType, _IntegralType _Value>
	struct TTraitConstant
	{
		static constexpr _IntegralType Value = _Value;
	};

	/*
		Trait constant, boolean version.
	*/
	template<bool Value>
	using TTraitBoolConstant = TTraitConstant<bool, Value>;

	/*
		Trait true type.
	*/
	using TTraitTrueType = TTraitBoolConstant<true>;

	/*
		Trait false type.
	*/
	using TTraitFalseType = TTraitBoolConstant<false>;

	/*
		Trait tester base.
	*/
	struct FTraitTesterBase
	{
		struct Yes { };
		struct No { };
	};

	/* Makes decl-value from given type. (not for real-use) */
	template<typename Type>
	constexpr Type DeclVal();

}