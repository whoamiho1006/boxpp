#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/IsPodType.hpp>

#include <boxpp/base/tpls/containers/ArrayBase.hpp>
#include <boxpp/base/tpls/containers/algorithms/BinarySearch.hpp>

namespace boxpp
{
	template<typename ElemType, bool bAscend = true>
	class TSortedArray : public TArrayBase<ElemType>
	{
	private:
		template<typename, bool> friend class TSortedArray;
		template<typename> friend class TArrayBase;

	public:
		TSortedArray(size_t InitialCapacity = 0) 
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
		FASTINLINE ssize_t Add(ssize_t Offset, const ElemType& Item, size_t Count = 1) {
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
		FASTINLINE ssize_t Add(ssize_t Offset, ElemType&& Item) {
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
		
	public:
		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE ssize_t Add(const ElemType& Item, size_t Count = 1) {
			if (this->Requires(Count)) {
				if (this->Length) {
					ssize_t Offset = ssize_t(TBinarySearch<ElemType, bAscend>::SearchNear(Item, this->Storage, this->Length));

					if (Offset >= 0) {
						return Add(Offset, Item, Count);
					}
				}

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
				if (this->Length) {
					ssize_t Offset = TBinarySearch<ElemType, bAscend>::SearchNear(Item, this->Storage, this->Length);

					if (Offset >= 0) {
						return Add(Offset, Forward<ElemType>(Item));
					}
				}

				new (this->Storage + this->Length++) ElemType(Forward<ElemType>(Item));
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
				return Add(Forward<ElemType>(Item));
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(const TArrayBase<ElemType>& InArray) {
			return Append(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t AppendUnique(const TArrayBase<ElemType>& InArray) {
			return AppendUnique(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(const ElemType* Items, size_t Count) {
			ssize_t Affected = 0;

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
		FASTINLINE ssize_t AppendUnique(const ElemType* Items, size_t Count) {
			ssize_t Affected = 0;

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
		FASTINLINE ssize_t Append(const TArrayBase<ElemType>& InArray, ssize_t Offset, size_t Count) {
			ssize_t Items = 0;

			for (ssize_t i = Offset; i < ssize_t(Count); i++) {
				if (Add(InArray[i]) >= 0)
					Items++;
			}

			return Items;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t AppendUnique(const TArrayBase<ElemType>& InArray, ssize_t Offset, size_t Count) {
			ssize_t Items = 0;

			for (ssize_t i = Offset; i < Count; i++) {
				if (AddUnique(InArray[i]) >= 0)
					Items++;
			}

			return Items;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE ssize_t Append(TArrayBase<ElemType>&& InArray) {
			ssize_t Items = 0;

			for (ssize_t i = 0; i < InArray.GetSize(); i++) {
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
			ssize_t Offset = IndexOf(Item);

			if (Offset >= 0) {
				return this->RemoveAt(Offset, 1, bOptimize);
			}

			return false;
		}

		/*
			Remove all item (or items) from this array.
		*/
		FASTINLINE ssize_t RemoveAll(const ElemType& Item, bool bOptimize = true) {
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
			return ssize_t(TBinarySearch<ElemType, bAscend>::Search(Item,
				this->Storage + Offset, this->Length - Offset));
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
		FASTINLINE ssize_t Search(const ElemType& Item) const {
			return TBinarySearch<ElemType, bAscend>::Search(Item, this->Storage, this->Length);
		}
	};
}
