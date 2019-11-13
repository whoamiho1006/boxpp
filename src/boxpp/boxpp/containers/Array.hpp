#ifndef __BOXPP_CONTAINERS_ARRAY_HPP__
#define __BOXPP_CONTAINERS_ARRAY_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/Movable.hpp>
#include <boxpp/containers/Iterator.hpp>

#include <boxpp/containers/algorithms/HeapSort.hpp>
#include <boxpp/containers/algorithms/BinarySearch.hpp>

namespace boxpp {
	/*
		Elem-Type should implement one of move-ctor or copy-ctor at least.
	*/
	template<typename ElemType>
	class TArray
	{
	private:
		template<typename> friend class TArray;
		typedef struct { } base_ctor_t;
		static constexpr base_ctor_t init_default = { };

	public:
		typedef TIterator<TArray<ElemType>, s32> Iterator;
		typedef TIterator<const TArray<ElemType>, s32> ConstIterator;

	private:
		/*
			Base initializer.
		*/
		TArray(base_ctor_t, u32 InitialCapacity)
			: Multiplier(1), Capacity(0), Length(0), Storage(nullptr)
		{
			if (InitialCapacity) {
				Requires(InitialCapacity);
			}
		}

	public:
		/*
			Initialize an array with initial Capacity.
		*/
		TArray(u32 GetCapacity = 0)
			: TArray(init_default, GetCapacity)
		{

		}

		/*
			Clones an array from original array.
		*/
		TArray(const TArray<ElemType>& InArray)
			: TArray(init_default, InArray.GetSize())
		{
			Append(InArray);
		}

		/*
			Shifts all resources from given array.
		*/
		TArray(TArray<ElemType>&& InArray)
			: TArray(init_default, 0)
		{
			Swap(Multiplier, InArray.Multiplier);
			Swap(Capacity, InArray.Capacity);
			Swap(Length, InArray.Length);
			Swap(Storage, InArray.Storage);
		}

		/*
			Releases internal memory of TArray.
		*/
		~TArray()
		{
			Clear();
		}

	private:
		u32 Multiplier;
		u32 Capacity;
		u32 Length;

	private:
		ElemType* Storage;

	public:
		FASTINLINE u32 GetMultiplier() const { return Multiplier; }
		FASTINLINE u32 GetCapacity() const { return Capacity; }
		FASTINLINE u32 GetSize() const { return Length; }

	public:
		FASTINLINE void SetMultiplier(u32 Value) { Multiplier = Value; }
		FASTINLINE ElemType* GetRaw() const { return Storage; }

	public:
		FASTINLINE operator bool() const { return Length; }
		FASTINLINE bool operator !() const { return !Length; }
		FASTINLINE bool operator ==(const TArray<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TArray<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE TArray<ElemType>& operator =(const TArray<ElemType>& Other) {
			if (*this != Other) {
				Clear();
				SetMultiplier(1);
				Requires(Other.GetSize());
				Length = Other.GetSize();

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage, Other.Storage, sizeof(ElemType) * Other.GetSize());
				}

				else {
					for (u32 i = 0; i < Other.GetSize(); i++) {
						new (&Storage[i]) ElemType(Other.Storage[i]);
					}
				}
			}

			return *this;
		}

		FASTINLINE TArray<ElemType>& operator =(TArray<ElemType>&& Other) {
			if (*this != Other) {
				Swap(Multiplier, Other.Multiplier);
				Swap(Capacity, Other.Capacity);
				Swap(Length, Other.Length);
				Swap(Storage, Other.Storage);
			}

			return *this;
		}

	public:
		FASTINLINE ElemType& operator[](s32 Offset) { return Storage[Offset]; }
		FASTINLINE const ElemType& operator[](s32 Offset) const { return Storage[Offset]; }
		FASTINLINE bool IsValid(s32 Offset) const { return Offset >= 0 && Offset < s32(Length); }

	public:
		/*
			Clear this array and release all memory.
		*/
		FASTINLINE void Clear() {
			if (Storage) {
				ElemType* Storage = nullptr;
				u32 GetCapacity = 0;
				u32 GetSize = 0;

				Swap(this->Storage, Storage);
				Swap(Capacity, GetCapacity);
				Swap(Length, GetSize);

				for (u32 i = 0; i < GetSize; i++) {
					Storage[i].~ElemType();
				}

				delete[]((u8*)Storage);
			}
		}

	private:
		/*
			Try to hold required size.
			Finally, this array will have (m_Uses + Size) capacity in minimal.
		*/
		FASTINLINE bool Requires(u32 Size) {
			if (Length + Size > Capacity) {
				ElemType* Storage = (ElemType*) new u8[
					(Length + Size) * Multiplier * sizeof(ElemType)];

				if (!Storage) {
					return false;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage, this->Storage, sizeof(ElemType) * Length);
				}

				else {
					for (u32 i = 0; i < Length; i++) {
						new (Storage + i) ElemType(TMovable<ElemType>::Movable(this->Storage[i]));
						this->Storage[i].~ElemType();
					}
				}

				Swap(this->Storage, Storage);
				Capacity = (Length + Size) * Multiplier;

				if (Storage) {
					delete[]((u8*)Storage);
				}
			}

			return true;
		}

	public:
		/*
			Try to un-hold unrequired size.
			Finally, this array will have (m_Uses * m_Multiplier) capacity in maximum.
		*/
		FASTINLINE void Optimize() {
			if (Storage && Length * Multiplier < Capacity) {
				ElemType* Storage = (ElemType*) new u8[
					Length * Multiplier * sizeof(ElemType)];

				if (!Storage) {
					return;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage, this->Storage, sizeof(ElemType) * Length);
				}
				else {
					for (u32 i = 0; i < Length; i++) {
						new (Storage + i) ElemType(TMovable<ElemType>::Movable(this->Storage[i]));
						this->Storage[i].~ElemType();
					}
				}

				Swap(this->Storage, Storage);
				Capacity = Length * Multiplier;

				if (Storage) {
					delete[]((u8*)Storage);
				}
			}
		}

	public:
		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(const ElemType& Item, u32 Count = 1) {
			if (Requires(Count)) {
				for (u32 i = 0; i < Count; i++) {
					new (Storage + Length + i) ElemType(Item);
				}

				Length += Count;
				return s32(Length - Count);
			}

			return -1;
		}

		/*
			Add an item into this array and returns its offset.
		*/
		FASTINLINE s32 Add(ElemType&& Item) {
			if (Requires(1)) {
				new (Storage + Length) ElemType(Item);
				Length++;

				return s32(Length - 1);
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
		FASTINLINE s32 Append(const TArray<ElemType>& InArray) {
			return Append(InArray, 0, InArray.GetSize());
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const ElemType* Items, u32 GetSize) {
			if (Items && GetSize) {
				if (!Requires(GetSize)) {
					return -1;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage + Length, Items, sizeof(ElemType) * GetSize);
				}
				else {
					for (u32 i = 0; i < GetSize; i++) {
						new (Storage + Length + i) ElemType(Items[i]);
					}
				}

				Length += GetSize;
				return s32(Length - GetSize);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(const TArray<ElemType>& InArray, s32 Offset, u32 Count) {
			if (InArray.GetSize()) {
				if (InArray.GetSize() <= u32(Offset)) {
					return -1;
				}

				Count = Count > InArray.GetSize() - Offset ?
					u32(InArray.GetSize() - Offset) : Count;

				if (!Requires(Count)) {
					return -1;
				}

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage + Length, InArray.GetRaw() + Offset, sizeof(ElemType) * Count);
				}
				else {
					for (u32 i = 0; i < Count; i++) {
						new (Storage + Length + i) ElemType(InArray.Storage[i + Offset]);
					}
				}

				Length += Count;
				return s32(Length - Count);
			}

			return -1;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE s32 Append(TArray<ElemType>&& InArray) {
			if (InArray.GetSize() && Requires(InArray.GetSize())) {
				s32 Offset = s32(Length);

				if (IsTypePOD<ElemType>) {
					::memcpy(Storage + Length, InArray.GetRaw(), sizeof(ElemType) * InArray.GetSize());
				}
				else {
					for (u32 i = 0; i < InArray.GetSize(); i++) {
						new (Storage + Length + i) ElemType(
							TMovable<ElemType>::Movable(InArray.Storage[i]));
					}
				}

				Length += InArray.GetSize();
				InArray.Clear();

				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, const ElemType& Item, u32 Count = 1) {
			if (Offset >= 0 && Requires(Count)) {
				if (IsTypePOD<ElemType>) {
					::memmove(Storage + Offset + Count, Storage + Offset,
						sizeof(ElemType) * (Length - Offset));
				}
				else {
					for (u32 i = Length; i > u32(Offset); i--) {
						new (Storage + i) ElemType(
							TMovable<ElemType>::Movable(Storage[i - Count])
						);

						Storage[i - Count].~ElemType();
					}
				}

				for (u32 i = 0; i < Count; i++)
					new (Storage + Offset + i) ElemType(Item);

				Length += Count;
				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, const ElemType* Items, u32 Count) {
			if (Offset >= 0 && Requires(Count)) {
				if (IsTypePOD<ElemType>) {
					::memmove(Storage + Offset + Count, Storage + Offset,
						sizeof(ElemType) * (Length - Offset));

					::memcpy(Storage + Offset, Items, sizeof(ElemType) * Count);
				}
				else {
					for (u32 i = Length; i > u32(Offset); i--) {
						new (Storage + i) ElemType(
							TMovable<ElemType>::Movable(Storage[i - Count])
						);

						Storage[i - Count].~ElemType();
					}

					for (u32 i = 0; i < Count; i++)
						new (Storage + Offset + i) ElemType(Items[i]);
				}

				Length += Count;
				return Offset;
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, const TArray<ElemType>& InArray) {
			if (Offset >= 0 && InArray.GetSize()) {
				return Insert(Offset, InArray.GetRaw(), InArray.GetSize());
			}

			return -1;
		}

		/*
			Insert an item into this array and returns its offset.
		*/
		FASTINLINE s32 Insert(s32 Offset, ElemType&& Item) {
			if (Offset >= 0 && Requires(1)) {
				if (IsTypePOD<ElemType>) {
					::memmove(Storage + Offset + 1, Storage + Offset,
						sizeof(ElemType) * (Length - Offset));
				}
				else {
					for (u32 i = Length; i > u32(Offset); i--) {
						new (Storage + i) ElemType(
							TMovable<ElemType>::Movable(Storage[i - 1])
						);

						Storage[i - 1].~ElemType();
					}
				}

				new (Storage + Offset) ElemType(Item);
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
				return RemoveAt(Offset, 1, bOptimize);
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
				Count += RemoveAt(Offset, 1, bOptimize) ? 1 : 0;
				Offset = IndexOf(Item, u32(Offset));
			}

			return Count;
		}

		/*
			Remove an item (or items) from this array.
		*/
		FASTINLINE bool RemoveAt(s32 Offset, u32 Count = 1, bool bOptimize = true) {
			if (Offset >= 0 && Offset < s32(Length)) {
				u32 FinalLength = u32(Length < Count ? 0 : Length - Count);

				if (IsTypePOD<ElemType>) {
					::memmove(Storage + Offset, Storage + Offset + Count,
						sizeof(ElemType) * (Length - (Offset + Count)));
				}
				else {
					for (u32 i = u32(Offset); i < FinalLength; i++) {
						Storage[i].~ElemType();

						new (Storage + i) ElemType(
							TMovable<ElemType>::Movable(Storage[i + Count])
						);
					}

					for (u32 i = FinalLength; i < Length; i++) {
						Storage[i].~ElemType();
					}
				}

				Length = FinalLength;

				if (bOptimize) {
					Optimize();
				}

				return true;
			}

			return false;
		}

	public:
		/*
			Find the index of given item.
		*/
		FASTINLINE s32 IndexOf(const ElemType& Item, u32 Offset = 0) const {
			for (u32 i = Offset; i < Length; i++) {
				if (TEquals<ElemType>::Equals(Item, Storage[i]))
					return int32(i);
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
			if (Length) {
				THeapSort<ElemType>::Sort(Storage, Length, bDescend);
			}
		}

		/*
			Search an item from this array using binary search.
			In this case, This array should be sorted in ascend order.
		*/
		FASTINLINE s32 Search(const ElemType& Item) const {
			return TBinarySearch<ElemType>::Search(Item, Storage, Length);
		}
	};

	template<typename ElemType>
	FASTINLINE typename TArray<ElemType>::Iterator begin(TArray<ElemType>& InArray)
	{
		return typename TArray<ElemType>::Iterator(InArray, 0);
	}

	template<typename ElemType>
	FASTINLINE typename TArray<ElemType>::Iterator end(TArray<ElemType>& InArray)
	{
		return typename TArray<ElemType>::Iterator(InArray, InArray.GetSize());
	}

	template<typename ElemType>
	FASTINLINE typename TArray<ElemType>::ConstIterator begin(const TArray<ElemType>& InArray)
	{
		return typename TArray<ElemType>::ConstIterator(InArray, 0);
	}

	template<typename ElemType>
	FASTINLINE typename TArray<ElemType>::ConstIterator end(const TArray<ElemType>& InArray)
	{
		return typename TArray<ElemType>::ConstIterator(InArray, InArray.GetSize());
	}
}

#endif // !__BOXPP_CONTAINERS_ARRAY_HPP__