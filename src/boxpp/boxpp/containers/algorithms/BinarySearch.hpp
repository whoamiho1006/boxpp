#ifndef __BOXPP_CONTAINERS_ALGORITHMS_BINARYSEARCH_HPP__
#define __BOXPP_CONTAINERS_ALGORITHMS_BINARYSEARCH_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/Movable.hpp>
#include <boxpp/utils/Comparator.hpp>

namespace boxpp {
	template<typename T, bool bAscend = true>
	struct TBinarySearch
	{
		using RangeType = PickType<bAscend, TRanged<T>, TInvertedRange<T>>;
		using LeftOver = PickType<bAscend, TSmaller<T>, TBigger<T>>;
		using RightOver = PickType<bAscend, TBigger<T>, TSmaller<T>>;

		FASTINLINE static ssize_t Search(const T& Item, T* Items, size_t Size)
		{
			if (Size) {
				size_t Left = 0;
				size_t Right = Size - 1;

				if (TEquals<T>::Compare(Item, Items[Left]))
					return ssize_t(Left);

				else if (TEquals<T>::Compare(Item, Items[Right]))
					return ssize_t(Right);

				while (Left != Right) {
					size_t Mid = (Left + Right) / 2;

					if (TEquals<T>::Compare(Item, Items[Mid]))
						return ssize_t(Mid);

					else if (Left != Mid &&
						RangeType::Ranged(Item, Items[Left], Items[Mid]))
					{
						if (Right == Mid)
							break;

						Right = Mid;
					}

					else if (Mid != Right &&
						RangeType::Ranged(Item, Items[Mid], Items[Right]))
					{
						if (Left == Mid)
							break;

						Left = Mid;
					}

					else break;
				}
			}

			return -1;
		}

		FASTINLINE static ssize_t SearchNear(const T& Item, T* Items, size_t Size)
		{
			if (Size) {
				size_t Left = 0;
				size_t Right = Size - 1;

				if (TEquals<T>::Compare(Item, Items[Left]))
					return ssize_t(Left);

				else if (TEquals<T>::Compare(Item, Items[Right]))
					return ssize_t(Right);

				else if (LeftOver::Compare(Item, Items[Left]))
					return ssize_t(Left);

				else if (RightOver::Compare(Item, Items[Right]))
					return ssize_t(Right + 1);

				while (Left != Right) {
					size_t Mid = (Left + Right) / 2;

					if (TEquals<T>::Compare(Item, Items[Mid]))
						return ssize_t(Mid);

					else if (Left != Mid &&
						RangeType::Ranged(Item, Items[Left], Items[Mid]))
					{
						if (Right == Mid)
							break;

						Right = Mid;
					}

					else if (Mid != Right &&
						RangeType::Ranged(Item, Items[Mid], Items[Right]))
					{
						if (Left == Mid)
							break;

						Left = Mid;
					}

					else break;
				}

				return (Left + Right) / 2 + (bAscend ? 0 : 1);
			}

			return -1;
		}
	};
}

#endif // !__BOXPP_CONTAINERS_ALGORITHMS_BINARYSEARCH_HPP__