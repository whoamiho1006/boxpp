#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>
#include <boxpp/base/tpls/traits/Movable.hpp>
#include <boxpp/base/tpls/containers/Comparator.hpp>

namespace boxpp {
	
	template<typename T, typename Comparator>
	struct TNormalizedHeapSort
	{
		FASTINLINE static void DownHeap(T* Items, ssize_t Cursor, ssize_t Offset) {
			ssize_t LeftOffset, RightOffset, Root;

			while (Cursor < Offset) {
				LeftOffset = Cursor * 2 + 1;
				RightOffset = Cursor * 2 + 2;

				if (LeftOffset >= Offset && RightOffset >= Offset)
					break;

				Root = Cursor;

				if (LeftOffset < Offset &&
					Comparator::Compare(Items[Root], Items[LeftOffset]))
				{
					Root = LeftOffset;
				}

				if (RightOffset < Offset &&
					Comparator::Compare(Items[Root], Items[RightOffset]))
				{
					Root = RightOffset;
				}

				if (Root == Cursor)
					break;

				Swap(Items[Cursor], Items[Root]);
				Cursor = Root;
			}
		}

		FASTINLINE static void Sort(T* Items, ssize_t Size)
		{
			if (!Size)
				return;

			for (ssize_t i = (Size - 1) / 2; i >= 0; i--) {
				DownHeap(Items, i, Size);
			}

			for (ssize_t i = (Size - 1); i > 0; i--) {
				Swap(Items[0], Items[i]);
				DownHeap(Items, 0, i);
			}
		}
	};

	template<typename T>
	using THeapSortDescend = TNormalizedHeapSort<T, TBigger<T>>;

	template<typename T>
	using THeapSortAscend = TNormalizedHeapSort<T, TSmaller<T>>;

	template<typename T>
	struct THeapSort
	{
		FASTINLINE static void Sort(T* Items, ssize_t Size, bool bDescend = false)
		{
			if (bDescend)
				THeapSortDescend<T>::Sort(Items, Size);

			else THeapSortAscend<T>::Sort(Items, Size);
		}
	};
}
