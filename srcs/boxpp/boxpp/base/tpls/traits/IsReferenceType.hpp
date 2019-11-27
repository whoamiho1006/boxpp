#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>

namespace boxpp
{
	/* Determines given type is LValue reference or not. (Easily, == Type&) */
	template<typename Type>
	struct TIsLeftReferenceType : TTraitBoolConstant<false> { };

	template<typename Type>
	struct TIsLeftReferenceType<Type&> : TTraitBoolConstant<true> { };

	/* Determines given type is RValue reference or not. (Easily, == Type&&) */
	template<typename Type>
	struct TIsRightReferenceType : TTraitBoolConstant<false> { };

	template<typename Type>
	struct TIsRightReferenceType<Type&&> : TTraitBoolConstant<true> { };

	/* Determines given type is reference or not. (Easily, == Type&&) */
	template<typename Type>
	struct TIsReferenceType : TTraitBoolConstant<false> { };

	template<typename Type>
	struct TIsReferenceType<Type&> : TTraitBoolConstant<true> { };

	template<typename Type>
	struct TIsReferenceType<Type&&> : TTraitBoolConstant<true> { };

	/* Determines given type is LValue reference or not. (Easily, == Type&) */
	template<typename Type>
	constexpr bool IsLeftReferenceType = TIsLeftReferenceType<Type>::Value;

	/* Determines given type is RValue reference or not. (Easily, == Type&&) */
	template<typename Type>
	constexpr bool IsRightReferenceType = TIsRightReferenceType<Type>::Value;

	/* Determines given type is reference or not. (Easily, == Type&&) */
	template<typename Type>
	constexpr bool IsReferenceType = TIsReferenceType<Type>::Value;
}