#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/IsPodType.hpp>
#include <boxpp/traits/Movable.hpp>
#include <boxpp/containers/Iterator.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TArray;

	template<typename ElemType, bool bAscend>
	class TSortedArray;

	template<typename ElemType>
	class TArrayBase
	{
	private:
		template<typename> friend class TArray;
		template<typename> friend class TArrayBase;
		template<typename, bool> friend class TSortedArray;

	public:
		typedef s32 OffsetType;
		typedef TIterator<TArrayBase<ElemType>> Iterator;
		typedef TIterator<const TArrayBase<ElemType>> ConstIterator;
		typedef TReverseIterator<TArrayBase<ElemType>> ReverseIterator;
		typedef TReverseIterator<const TArrayBase<ElemType>> ConstReverseIterator;

	protected:
		TArrayBase(u32 InitialCapacity = 0)
			: Multiplier(1), Capacity(0), Length(0), Storage(nullptr) 
		{
			if (InitialCapacity) {
				Requires(InitialCapacity);
			}
		}

		virtual ~TArrayBase() {
			Clear();
		}

	protected:
		u32 Multiplier;
		u32 Capacity;
		u32 Length;

	protected:
		ElemType* Storage;

	public:
		FASTINLINE operator bool() const { return Length; }
		FASTINLINE bool operator !() const { return !Length; }

		FASTINLINE bool IsEmpty() const { return !Length; }

		FASTINLINE bool operator ==(const TArrayBase<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TArrayBase<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE u32 GetMultiplier() const { return Multiplier; }
		FASTINLINE u32 GetCapacity() const { return Capacity; }
		FASTINLINE u32 GetSize() const { return Length; }

	public:
		FASTINLINE void SetMultiplier(u32 Value) { Multiplier = Value; }
		FASTINLINE ElemType* GetRaw() const { return Storage; }

	public:
		FASTINLINE ElemType& operator[](s32 Offset) { return Storage[Offset]; }
		FASTINLINE const ElemType& operator[](s32 Offset) const { return Storage[Offset]; }
		FASTINLINE bool IsValid(s32 Offset) const { return Offset >= 0 && Offset < s32(Length); }

	public:
		FASTINLINE Iterator Begin() const { return Iterator(*this, 0); }
		FASTINLINE Iterator End() const { return Iterator(*this, Length); }

	protected:
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

				if (IsPodType<ElemType>) {
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

				if (IsPodType<ElemType>) {
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

		/*
			Clear this array and release all memory.
		*/
		FASTINLINE void Clear(bool bKeepSlack = false) {
			if (Storage) {
				if (bKeepSlack) {
					for (u32 i = 0; i < Length; i++) {
						Storage[i].~ElemType();
					}

					this->Length = 0;
				}
				else {
					ElemType* Storage = nullptr;
					u32 Capacity = 0;
					u32 Length = 0;

					Swap(this->Storage, Storage);
					Swap(this->Capacity, Capacity);
					Swap(this->Length, Length);

					for (u32 i = 0; i < Length; i++) {
						Storage[i].~ElemType();
					}

					delete[]((u8*)Storage);
				}
			}
		}

		/*
			Remove an item (or items) from this array.
		*/
		FASTINLINE bool RemoveAt(s32 Offset, u32 Count = 1, bool bOptimize = true) {
			if (Offset >= 0 && Offset < s32(Length)) {
				u32 FinalLength = u32(Length < Count ? 0 : Length - Count);

				if (IsPodType<ElemType>) {
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


		/*
			Find item index using predicate.
		*/
		template<typename PredicateType>
		FASTINLINE s32 RemoveLike(PredicateType&& Predicate, u32 Offset = 0, bool bOptimize = true) {
			u32 Count = 0;
			u32 i = Offset;

			while (i < Length) {
				if (Predicate(Storage[i]))
					RemoveAt(i, 1, false);

				else ++i;
			}

			if (bOptimize)
				Optimize();

			return Count;
		}

		/*
			Find item index using predicate.
		*/
		template<typename PredicateType>
		FASTINLINE s32 Find(PredicateType&& Predicate, u32 Offset = 0) const {
			for (u32 i = Offset; i < Length; i++) {
				if (Predicate(Storage[i]))
					return s32(i);
			}

			return -1;
		}
	};

	template<typename ElemType>
	using TArrayIterator = typename TArrayBase<ElemType>::Iterator;

	template<typename ElemType>
	using TArrayConstIterator = typename TArrayBase<ElemType>::ConstIterator;

	template<typename ElemType>
	using TArrayReverseIterator = typename TArrayBase<ElemType>::ReverseIterator;

	template<typename ElemType>
	using TArrayConstReverseIterator = typename TArrayBase<ElemType>::ConstReverseIterator;

	template<typename ElemType>
	FASTINLINE TArrayIterator<ElemType> begin(TArrayBase<ElemType>& InArray)
	{
		return TArrayIterator<ElemType>(InArray, 0);
	}

	template<typename ElemType>
	FASTINLINE TArrayIterator<ElemType> end(TArrayBase<ElemType>& InArray)
	{
		return TArrayIterator<ElemType>(InArray, InArray.GetSize());
	}

	template<typename ElemType>
	FASTINLINE TArrayConstIterator<ElemType> begin(const TArrayBase<ElemType>& InArray)
	{
		return TArrayConstIterator<ElemType>(InArray, 0);
	}

	template<typename ElemType>
	FASTINLINE TArrayConstIterator<ElemType> end(const TArrayBase<ElemType>& InArray)
	{
		return TArrayConstIterator<ElemType>(InArray, InArray.GetSize());
	}

	template<typename ElemType>
	FASTINLINE TArrayReverseIterator<ElemType> rbegin(TArrayBase<ElemType>& InArray)
	{
		return TArrayIterator<ElemType>(InArray, InArray.GetSize() - 1);
	}

	template<typename ElemType>
	FASTINLINE TArrayReverseIterator<ElemType> rend(TArrayBase<ElemType>& InArray)
	{
		return TArrayIterator<ElemType>(InArray, -1);
	}

	template<typename ElemType>
	FASTINLINE TArrayConstReverseIterator<ElemType> rbegin(const TArrayBase<ElemType>& InArray)
	{
		return TArrayConstIterator<ElemType>(InArray, InArray.GetSize() - 1);
	}

	template<typename ElemType>
	FASTINLINE TArrayConstReverseIterator<ElemType> rend(const TArrayBase<ElemType>& InArray)
	{
		return TArrayConstIterator<ElemType>(InArray, -1);
	}
}
