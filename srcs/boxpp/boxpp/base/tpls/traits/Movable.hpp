#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>
#include <boxpp/base/tpls/traits/IsConstructibleType.hpp>

namespace boxpp
{
	/*	Make movable reference if possible. If given type has no move-constructor,
		This returns const-reference for making it to be copied. */
	template<typename T, bool IsMovable = IsMoveConstructibleType<T>>
	struct TMovable
	{
		/* If type T is Move-constructible, returns move-constructible reference. */
		FASTINLINE static T&& Movable(T& Reference) {
			return static_cast<T&&>(Reference);
		}
	};

	template<typename T>
	struct TMovable<T, false>
	{
		/* If type T is NOT Move-constructible, returns copy-constructible reference. */
		FASTINLINE static const T& Movable(T& Reference) {
			return Reference;
		}
	};

	/* Swap between left and right. */
	template<typename T>
	FASTINLINE static T& Swap(T& Left, T& Right) {
		T Temp(TMovable<T>::Movable(Left));
		Left = TMovable<T>::Movable(Right);
		Right = TMovable<T>::Movable(Temp);
		return Left;
	}
}