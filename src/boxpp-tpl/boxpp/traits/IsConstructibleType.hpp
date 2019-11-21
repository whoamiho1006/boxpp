#pragma once
#include <boxpp/BaseTraits.hpp>

namespace boxpp
{
	/* Determines given type can be constructible by given arguments. */
	template<typename CallableType, typename ... ArgTypes>
	struct TIsConstructibleType : TTraitBoolConstant<__is_constructible(CallableType, ArgTypes ...)> { };

	/* Constexpr Value version of TIsConstructible. */
	template<typename CallableType, typename ... ArgTypes>
	constexpr bool IsConstructibleType = TIsConstructibleType<CallableType, ArgTypes ...>::Value;

	/* Determines given type is move-constructible or not. */
	template<typename CallableType>
	using TIsMoveConstructibleType = TIsConstructibleType<CallableType, CallableType&&>;

	/* Determines given type is copy-constructible or not. */
	template<typename CallableType>
	using TIsCopyConstructibleType = TIsConstructibleType<CallableType, const CallableType&>;

	/* Constexpr Value version of TIsMoveConstructible. */
	template<typename CallableType>
	constexpr bool IsMoveConstructibleType = TIsMoveConstructibleType<CallableType>::Value;

	/* Constexpr Value version of TIsCopyConstructible. */
	template<typename CallableType>
	constexpr bool IsCopyConstructibleType = TIsCopyConstructibleType<CallableType>::Value;
}