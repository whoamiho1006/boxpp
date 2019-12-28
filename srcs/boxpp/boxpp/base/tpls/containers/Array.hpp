#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/Forward.hpp>
#include <boxpp/base/tpls/containers/ArrayBase.hpp>
#include <boxpp/base/tpls/containers/algorithms/BinarySearch.hpp>
#include <boxpp/base/tpls/containers/algorithms/HeapSort.hpp>

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
		TArray(size_t InitialCapacity = 0)
			: TArrayBase<ElemType>(InitialCapacity)
		{
		}

		/*
			Clones an array from original array.
		*/
		TArray(const TArray<ElemType>& InArray)
			: TArrayBase<ElemType>(InArray.GetSize())
		{
			Append(InArray);
		}

		/*
			Shifts all resources from given array.
		*/
		TArray(TArray<ElemType>&& InArray)
			: TArrayBase<ElemType>(0)
		{
			Swap(this->Multiplier, InArray.Multiplier);
			Swap(this->Capacity, InArray.Capacity);
			Swap(this->Length, InArray.Length);
			Swap(this->Storage, InArray.Storage);
		}

	public:
		FASTINLINE TArray<ElemType>& operator =(const TArray<ElemType>& Other) {
			if (*this != Other) {
				this->Clear();
				this->SetMultiplier(1);
				this->Requires(Other.GetSize());
				this->Length = Other.GetSize();

				if (IsPodType<ElemType>) {
					::memcpy(this->Storage, Other.Storage, sizeof(ElemType) * Other.GetSize());
				}

				else {
					for (size_t i = 0; i < Other.GetSize(); i++) {
						new (&this->Storage[i]) ElemType(Other.Storage[i]);
					}
				}
			}

			return *this;
		}

		FASTINLINE TArray<ElemType>& operator =(TArray<ElemType>&& Other) {
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
		FASTINLINE ssize_t Add(const ElemType& Item, size_t Count = 1) {
			if (this->Requires(Count)) {
				for (size_t i = 0; i < Count; i++) {
					new (this->Storage + this->Length + i) ElemType(Item);
				}

				this->Length += Count;
				return ssize_t(this->Length - Count);
			}

			return -1;
		}

		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE ssize_t Add(ElemType&& Item) {
			if (this->Requires(1)) {
				new (this->Storage + this->Length) ElemType(Forward<ElemType>(Item));
				this->Length++;

				return ssize_t(this->Length - 1);
			}

			return -1;
		}

		/*
			Add an item uniquely into this array and returns its offset.
		*/
		FASTINLINE ssize_t AddUnique(const ElemType& Item) {
			if (!Contains(Item)) {
				return Add(Item);
			}

			return -1;
		}

		/*
			Add an item uniquely into this array and returns its offset.
		*/
		FASTINLINE ssize_t AddUnique(ElemType&& Item) {
			if (!Contains(Item)) {
				return Add(Item);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(const TArray<ElemType>& InArray) {
			return Append(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(const ElemType* Items, size_t Count) {
			if (Items && Count) {
				if (!this->Requires(Count)) {
					return -1;
				}

				if (IsPodType<ElemType>) {
					::memcpy(this->Storage + this->Length, Items, sizeof(ElemType) * Count);
				}
				else {
					for (size_t i = 0; i < Count; i++) {
						new (this->Storage + this->Length + i) ElemType(Items[i]);
					}
				}

				this->Length += Count;
				return ssize_t(this->Length - Count);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(const TArray<ElemType>& InArray, ssize_t Offset, size_t Count) {
			if (InArray.GetSize()) {
				if (InArray.GetSize() <= size_t(Offset)) {
					return -1;
				}

				Count = Count > InArray.GetSize() - Offset ?
					size_t(InArray.GetSize() - Offset) : Count;

				if (!this->Requires(Count)) {
					return -1;
				}

				if (IsPodType<ElemType>) {
					::memcpy(this->Storage + this->Length, InArray.GetRaw() + Offset, sizeof(ElemType) * Count);
				}
				else {
					for (size_t i = 0; i < Count; i++) {
						new (this->Storage + this->Length + i) ElemType(InArray.Storage[i + Offset]);
					}
				}

				this->Length += Count;
				return ssize_t(this->Length - Count);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(TArray<ElemType>&& InArray) {
			if (InArray.GetSize() && this->Requires(InArray.GetSize())) {
				ssize_t Offset = ssize_t(this->Length);

				if (IsPodType<ElemType>) {
					::memcpy(this->Storage + this->Length, InArray.GetRaw(), sizeof(ElemType) * InArray.GetSize());
				}
				else {
					for (size_t i = 0; i < InArray.GetSize(); i++) {
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
		FASTINLINE ssize_t Insert(ssize_t Offset, const ElemType& Item, size_t Count = 1) {
			if (Offset >= 0 && this->Requires(Count)) {
				if (IsPodType<ElemType>) {
					::memmove(this->Storage + Offset + Count, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));
				}
				else {
					for (size_t i = this->Length; i > size_t(Offset); i--) {
						new (this->Storage + i) ElemType(
							TMovable<ElemType>::Movable(this->Storage[i - Count])
						);

						this->Storage[i - Count].~ElemType();
					}
				}

				for (size_t i = 0; i < Count; i++)
					new (this->Storage + Offset + i) ElemType(Item);

				this->Length += Count;
				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE ssize_t Insert(ssize_t Offset, const ElemType* Items, size_t Count) {
			if (Offset >= 0 && this->Requires(Count)) {
				if (IsPodType<ElemType>) {
					::memmove(this->Storage + Offset + Count, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));

					::memcpy(this->Storage + Offset, Items, sizeof(ElemType) * Count);
				}
				else {
					for (size_t i = this->Length; i > size_t(Offset); i--) {
						new (this->Storage + i) ElemType(
							TMovable<ElemType>::Movable(this->Storage[i - Count])
						);

						this->Storage[i - Count].~ElemType();
					}

					for (size_t i = 0; i < Count; i++)
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
		FASTINLINE ssize_t Insert(ssize_t Offset, const TArray<ElemType>& InArray) {
			if (Offset >= 0 && InArray.GetSize()) {
				return Insert(Offset, InArray.GetRaw(), InArray.GetSize());
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE ssize_t Insert(ssize_t Offset, ElemType&& Item) {
			if (Offset >= 0 && this->Requires(1)) {
				if (IsPodType<ElemType>) {
					::memmove(this->Storage + Offset + 1, this->Storage + Offset,
						sizeof(ElemType) * (this->Length - Offset));
				}
				else {
					for (size_t i = this->Length; i > size_t(Offset); i--) {
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
			ssize_t Offset = IndexOf(Item);

			if (Offset >= 0) {
				return this->RemoveAt(Offset, 1, bOptimize);
			}

			return false;
		}

		/*
			Remove all item (or items) from this array.
		*/
		FASTINLINE size_t RemoveAll(const ElemType& Item, bool bOptimize = true) {
			size_t Count = 0;
			ssize_t Offset = IndexOf(Item);

			while (Offset >= 0) {
				Count += this->RemoveAt(Offset, 1, false) ? 1 : 0;
				Offset = IndexOf(Item, size_t(Offset));
			}

			if (bOptimize)
				this->Optimize();

			return Count;
		}

	public:
		/*
			Find the index of given item.
		*/
		FASTINLINE ssize_t IndexOf(const ElemType& Item, size_t Offset = 0) const {
			for (size_t i = Offset; i < this->Length; i++) {
				if (!Compare(Item, this->Storage[i]))
					return ssize_t(i);
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
		FASTINLINE ssize_t Search(const ElemType& Item, bool bDescend = false) const {
			if (bDescend)
				return TBinarySearch<ElemType, false>::Search(Item, this->Storage, this->Length);

			return TBinarySearch<ElemType>::Search(Item, this->Storage, this->Length);
		}
	};
}
