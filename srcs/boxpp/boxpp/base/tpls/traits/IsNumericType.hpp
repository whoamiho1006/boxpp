#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>
#include <boxpp/base/tpls/traits/IsSameType.hpp>

namespace boxpp
{
	template<typename Type>
	struct TIsNumericType : TTraitBoolConstant<false> { };

	template<> struct TIsNumericType<s8> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<s16> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<s32> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<s64> : TTraitBoolConstant<true> { };

	template<> struct TIsNumericType<u8> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<u16> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<u32> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<u64> : TTraitBoolConstant<true> { };

	template<> struct TIsNumericType<f32> : TTraitBoolConstant<true> { };
	template<> struct TIsNumericType<f64> : TTraitBoolConstant<true> { };

	template<typename Type>
	constexpr bool IsNumericType = TIsNumericType<Type>::Value;
}