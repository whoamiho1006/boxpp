#ifndef __BOXPP_UTILS_MOVABLE_HPP__
#define __BOXPP_UTILS_MOVABLE_HPP__

#ifndef __BOXPP_HPP__
/* Loads boxpp.hpp header if not loaded. */
#include <boxpp.hpp>
#endif

namespace boxpp
{
	/* Determines given type is move-constructible or not. (Simple wrapper for compiler routine) */
	template<typename T>
	constexpr bool IsMoveConstructible = __is_constructible(T, T);

	/* Determines given type is copy-constructible or not. (Simple wrapper for compiler routine) */
	template<typename T>
	constexpr bool IsCopyConstructible = __is_constructible(T, const T&);

	/*	Make movable reference if possible. If given type has no move-constructor,
		This returns const-reference for making it to be copied. */
	template<typename T, bool IsMovable = IsMoveConstructible<T>>
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

	template<typename T>
	FASTINLINE static T& Swap(T& Left, T& Right) {
		T Temp(TMovable<T>::Movable(Left));
		Left = TMovable<T>::Movable(Right);
		Right = TMovable<T>::Movable(Temp);
		return Left;
	}
}
#endif // !__BOXPP_UTILS_MOVABLE_HPP__