#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	template<typename T>
	struct TBypassType { using Type = const T&; };

	template<typename T>
	struct TBypassType<T&> { using Type = T & ; };

	template<typename T>
	struct TBypassType<const T&> { using Type = const T&; };

	template<typename T>
	struct TBypassType<volatile T&> { using Type = volatile T&; };

	template<typename T>
	struct TBypassType<const volatile T&> { using Type = const volatile T&; };

	template<typename T>
	struct TBypassType<T&&> { using Type = T && ; };

	template<typename T>
	using BypassType = typename TBypassType<T>::Type;

	template<typename T>
	FASTINLINE BypassType<T> Bypass(BypassType<T> Value) { return Value; }
}