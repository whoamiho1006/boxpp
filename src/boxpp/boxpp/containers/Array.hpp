#ifndef __BOXPP_CONTAINERS_ARRAY_HPP__
#define __BOXPP_CONTAINERS_ARRAY_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/containers/ArrayBase.hpp>

#include <boxpp/containers/algorithms/HeapSort.hpp>
#include <boxpp/containers/algorithms/BinarySearch.hpp>

namespace boxpp {
	/*
		Elem-Type should implement one of move-ctor or copy-ctor at least.
	*/
	template<typename ElemType>
	class TArray : public TArrayBase<ElemType>
	{
	private:
		template<typename> friend class TArray;
		template<typename> friend class TArrayBase;

	public:
		/*
			Initialize an array with initial Capacity.
		*/
		TArray(u32 InitialCapacity = 0)
			: TArrayBase<ElemType>(InitialCapacity)
		{
		}

		/*
			Clones an array from original array.
		*/
		TArray(const TArrayBase<ElemType>& InArray)
			: TArrayBase<ElemType>(InArray.GetSize())
		{
			Append(InArray);
		}

		/*
			Shifts all resources from given array.
		*/
		TArray(TArrayBase<ElemType>&& InArray) {
			Swap(this->Multiplier, InArray.Multiplier);
			Swap(this->Capacity, InArray.Capacity);
			Swap(this->Length, InArray.Length);
			Swap(this->Storage, InArray.Storage);
		}

	public:
		FASTINLINE TArray<ElemType>& operator =(const TArrayBase<ElemType>& Other) {
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

		FASTINLINE TArray<ElemType>& operator =(TArrayBase<ElemType>&& Other) {
			if (*this != Other) {
				Swap(this->Multiplier, Other.Multiplier);
				Swap(this->Capacity, Other.Capacity);
				Swap(this->Length, Other.Length);
				Swap(this->Storage, Other.Storage);
			}

			return *this;
		}
		
	public:
		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(const ElemType& Item, u32 Count = 1) {
			if (this->Requires(Count)) {
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
				new (this->Storage + this->Length) ElemType(Item);
				this->Length++;

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
				return Add(Item);
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
		FASTINLINE s32 Append(const ElemType* Items, u32 Count) {
			if (Items && Count) {
				if (!this->Requires(Count)) {
					return -1;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(this->Storage + this->Length, Items, sizeof(ElemType) * Count);
				}
				else {
					for (u32 i = 0; i < Count; i++) {
						new (this->Storage + this->Length + i) ElemType(Items[i]);
					}
				}

				this->Length += Count;
				return s32(this->Length - Count);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const TArrayBase<ElemType>& InArray, s32 Offset, u32 Count) {
			if (InArray.GetSize()) {
				if (InArray.GetSize() <= u32(Offset)) {
					return -1;
				}

				Count = Count > InArray.GetSize() - Offset ?
					u32(InArray.GetSize() - Offset) : Count;

				if (!this->Requires(Count)) {
					return -1;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(this->Storage + this->Length, InArray.GetRaw() + Offset, sizeof(ElemType) * Count);
				}
				else {
					for (u32 i = 0; i < Count; i++) {
						new (this->Storage + this->Length + i) ElemType(InArray.Storage[i + Offset]);
					}
				}

				this->Length += Count;
				return s32(this->Length - Count);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(TArrayBase<ElemType>&& InArray) {
			if (InArray.GetSize() && Requires(InArray.GetSize())) {
				s32 Offset = s32(this->Length);

				if (IsTypePOD<ElemType>) {
					::memcpy(this->Storage + this->Length, InArray.GetRaw(), sizeof(ElemType) * InArray.GetSize());
				}
				else {
					for (u32 i = 0; i < InArray.GetSize(); i++) {
						new (this->Storage + this->Length + i) ElemType(
							TMovable<ElemType>::Movable(InArray.Storage[i]));
					}
				}

				this->Length += InArray.GetSize();
				InArray.Clear();

				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, const ElemType& Item, u32 Count = 1) {
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
		FASTINLINE s32 Insert(s32 Offset, const ElemType* Items, u32 Count) {
			if (Offset >= 0 && this->Requires(Count)) {
				if (IsTypePOD<ElemType>) {
					::memmove(this->Storage + Offset + Count, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));

					::memcpy(this->Storage + Offset, Items, sizeof(ElemType) * Count);
				}
				else {
					for (u32 i = this->Length; i > u32(Offset); i--) {
						new (this->Storage + i) ElemType(
							TMovable<ElemType>::Movable(this->Storage[i - Count])
						);

						this->Storage[i - Count].~ElemType();
					}

					for (u32 i = 0; i < Count; i++)
						new (this->Storage + Offset + i) ElemType(Items[i]);
				}

				this->Length += Count;
				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, const TArrayBase<ElemType>& InArray) {
			if (Offset >= 0 && InArray.GetSize()) {
				return Insert(Offset, InArray.GetRaw(), InArray.GetSize());
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, ElemType&& Item) {
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
			for (u32 i = Offset; i < this->Length; i++) {
				if (TEquals<ElemType>::Equals(Item, this->Storage[i]))
					return s32(i);
			}

			return -1;
		}

		/*
			Determines this array contains given item or not.
		*/
		FASTINLINE bool Contains(const ElemType& Item) const {
			return IndexOf(Item) >= 0;
		}

		/*
			Sort this array using heapsort.
		*/
		FASTINLINE void Sort(bool bDescend = false) {
			if (this->Length) {
				THeapSort<ElemType>::Sort(this->Storage, this->Length, bDescend);
			}
		}

		/*
			Search an item from this array using binary search.
			In this case, This array should be sorted in ascend order.
		*/
		FASTINLINE s32 Search(const ElemType& Item, bool bDescend = false) const {
			if (bDescend)
				return TBinarySearch<ElemType, false>::Search(Item, this->Storage, this->Length);

			return TBinarySearch<ElemType>::Search(Item, this->Storage, this->Length);
		}
	};
}

#endif // !__BOXPP_CONTAINERS_ARRAY_HPP__