#ifndef __BOXPP_CONTAINERS_SORTEDARRAY_HPP__
#define __BOXPP_CONTAINERS_SORTEDARRAY_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/containers/ArrayBase.hpp>
#include <boxpp/containers/algorithms/BinarySearch.hpp>

namespace boxpp
{
	template<typename ElemType, bool bAscend = true>
	class TSortedArray : public TArrayBase<ElemType>
	{
	private:
		template<typename, bool> friend class TSortedArray;
		template<typename> friend class TArrayBase;

	public:
		TSortedArray(u32 InitialCapacity = 0) 
			: TArrayBase<ElemType>(InitialCapacity)
		{

		}


		/*
			Clones an array from original array.
		*/
		TSortedArray(const TSortedArray<ElemType>& InArray)
			: TArrayBase<ElemType>(InArray.GetSize())
		{
			Append(InArray);
		}

		/*
			Shifts all resources from given array.
		*/
		TSortedArray(TSortedArray<ElemType>&& InArray) {
			Swap(this->Multiplier, InArray.Multiplier);
			Swap(this->Capacity, InArray.Capacity);
			Swap(this->Length, InArray.Length);
			Swap(this->Storage, InArray.Storage);
		}

	public:
		FASTINLINE TSortedArray<ElemType>& operator =(const TSortedArray<ElemType>& Other) {
			if (*this != Other) {
				this->Clear();
				this->SetMultiplier(1);
				this->Requires(Other.GetSize());
				this->Length = Other.GetSize();

				if (IsTypePOD<ElemType>) {
					::memcpy(this->Storage, Other.Storage, sizeof(ElemType) * Other.GetSize());
				}

				else {
					for (u32 i = 0; i < Other.GetSize(); i++) {
						new (&this->Storage[i]) ElemType(Other.Storage[i]);
					}
				}
			}

			return *this;
		}

		FASTINLINE TSortedArray<ElemType>& operator =(TSortedArray<ElemType>&& Other) {
			if (*this != Other) {
				Swap(this->Multiplier, Other.Multiplier);
				Swap(this->Capacity, Other.Capacity);
				Swap(this->Length, Other.Length);
				Swap(this->Storage, Other.Storage);
			}

			return *this;
		}

	private:
		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(s32 Offset, const ElemType& Item, u32 Count = 1) {
			if (Offset >= 0 && this->Requires(Count)) {
				if (IsTypePOD<ElemType>) {
					::memmove(this->Storage + Offset + Count, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));
				}
				else {
					for (u32 i = this->Length; i > u32(Offset); i--) {
						new (this->Storage + i) ElemType(
							TMovable<ElemType>::Movable(this->Storage[i - Count])
						);

						this->Storage[i - Count].~ElemType();
					}
				}

				for (u32 i = 0; i < Count; i++)
					new (this->Storage + Offset + i) ElemType(Item);

				this->Length += Count;
				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(s32 Offset, ElemType&& Item) {
			if (Offset >= 0 && this->Requires(1)) {
				if (IsTypePOD<ElemType>) {
					::memmove(this->Storage + Offset + 1, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));
				}
				else {
					for (u32 i = this->Length; i > u32(Offset); i--) {
						new (this->Storage + i) ElemType(
							TMovable<ElemType>::Movable(this->Storage[i - 1])
						);

						this->Storage[i - 1].~ElemType();
					}
				}

				new (this->Storage + Offset) ElemType(Item);
				++this->Length;
				return Offset;
			}

			return -1;
		}
		
	public:
		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(const ElemType& Item, u32 Count = 1) {
			if (this->Requires(Count)) {
				if (this->Length) {
					s32 Offset = TBinarySearch<ElemType, bAscend>::SearchNear(Item, this->Storage, this->Length);

					if (Offset >= 0) {
						return Add(Offset, Item, Count);
					}
				}

				for (u32 i = 0; i < Count; i++) {
					new (this->Storage + this->Length + i) ElemType(Item);
				}

				this->Length += Count;
				return s32(this->Length - Count);
			}

			return -1;
		}

		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(ElemType&& Item) {
			if (this->Requires(1)) {
				if (this->Length) {
					s32 Offset = TBinarySearch<ElemType, bAscend>::SearchNear(Item, this->Storage, this->Length);

					if (Offset >= 0) {
						return Add(Offset, Forward<ElemType>(Item));
					}
				}

				new (this->Storage + this->Length++) ElemType(Forward<ElemType>(Item));
				return s32(this->Length - 1);
			}

			return -1;
		}

		/*
			Add an item uniquely into this array and returns its offset.
		*/
		FASTINLINE s32 AddUnique(const ElemType& Item) {
			if (!Contains(Item)) {
				return Add(Item);
			}

			return -1;
		}

		/*
			Add an item uniquely into this array and returns its offset.
		*/
		FASTINLINE s32 AddUnique(ElemType&& Item) {
			if (!Contains(Item)) {
				return Add(Forward<ElemType>(Item));
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const TArrayBase<ElemType>& InArray) {
			return Append(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 AppendUnique(const TArrayBase<ElemType>& InArray) {
			return AppendUnique(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const ElemType* Items, u32 Count) {
			s32 Affected = 0;

			if (Items && Count) {

				while (Count)
				{
					if (Add(*Items) >= 0)
						++Affected;

					--Count;
					++Items;
				}
			}

			return Affected;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 AppendUnique(const ElemType* Items, u32 Count) {
			s32 Affected = 0;

			if (Items && Count) {

				while (Count)
				{
					if (AddUnique(*Items) >= 0)
						++Affected;

					--Count;
					++Items;
				}
			}

			return Affected;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const TArrayBase<ElemType>& InArray, s32 Offset, u32 Count) {
			s32 Items = 0;

			for (s32 i = Offset; i < Count; i++) {
				if (Emplace(InArray[i]) >= 0)
					Items++;
			}

			return Items;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 AppendUnique(const TArrayBase<ElemType>& InArray, s32 Offset, u32 Count) {
			s32 Items = 0;

			for (s32 i = Offset; i < Count; i++) {
				if (AddUnique(InArray[i]) >= 0)
					Items++;
			}

			return Items;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(TArrayBase<ElemType>&& InArray) {
			s32 Items = 0;

			for (s32 i = 0; i < InArray.GetSize(); i++) {
				if (Emplace(TMovable<ElemType>::Movable(InArray[i])) >= 0)
					Items++;
			}

			InArray.Clear();
			return Items;
		}

		/*
			Remove an item from this array.
		*/
		FASTINLINE bool Remove(const ElemType& Item, bool bOptimize = true) {
			s32 Offset = IndexOf(Item);

			if (Offset >= 0) {
				return this->RemoveAt(Offset, 1, bOptimize);
			}

			return false;
		}

		/*
			Remove all item (or items) from this array.
		*/
		FASTINLINE s32 RemoveAll(const ElemType& Item, bool bOptimize = true) {
			u32 Count = 0;
			s32 Offset = IndexOf(Item);

			while (Offset >= 0) {
				Count += this->RemoveAt(Offset, 1, false) ? 1 : 0;
				Offset = IndexOf(Item, u32(Offset));
			}

			if (bOptimize)
				this->Optimize();

			return Count;
		}

	public:
		/*
			Find the index of given item.
		*/
		FASTINLINE s32 IndexOf(const ElemType& Item, u32 Offset = 0) const {
			return TBinarySearch<ElemType, bAscend>::Search(Item, 
				this->Storage + Offset, this->Length - Offset);
		}

		/*
			Determines this array contains given item or not.
		*/
		FASTINLINE bool Contains(const ElemType& Item) const {
			return Search(Item) >= 0;
		}

		/*
			Search an item from this array using binary search.
			In this case, This array should be sorted in ascend order.
		*/
		FASTINLINE s32 Search(const ElemType& Item) const {
			return TBinarySearch<ElemType, bAscend>::Search(Item, this->Storage, this->Length);
		}
	};
}

#endif // !__BOXPP_CONTAINERS_SORTEDARRAY_HPP__