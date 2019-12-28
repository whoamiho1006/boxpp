#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/IsPodType.hpp>
#include <boxpp/base/tpls/traits/Movable.hpp>
#include <boxpp/base/tpls/containers/Iterator.hpp>

#include <boxpp/base/systems/Debugger.hpp>

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
		typedef ssize_t OffsetType;
		typedef TIterator<TArrayBase<ElemType>> Iterator;
		typedef TIterator<const TArrayBase<ElemType>> ConstIterator;
		typedef TReverseIterator<TArrayBase<ElemType>> ReverseIterator;
		typedef TReverseIterator<const TArrayBase<ElemType>> ConstReverseIterator;

	protected:
		TArrayBase(size_t InitialCapacity = 0)
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
		size_t Multiplier;
		size_t Capacity;
		size_t Length;

	protected:
		ElemType* Storage;

	public:
		FASTINLINE operator bool() const { return Length; }
		FASTINLINE bool operator !() const { return !Length; }

		FASTINLINE bool IsEmpty() const { return !Length; }

		FASTINLINE bool operator ==(const TArrayBase<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TArrayBase<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE size_t GetMultiplier() const { return Multiplier; }
		FASTINLINE size_t GetCapacity() const { return Capacity; }
		FASTINLINE size_t GetSize() const { return Length; }

	public:
		FASTINLINE void SetMultiplier(size_t Value) { Multiplier = Value; }
		FASTINLINE ElemType* GetRaw() const { return Storage; }

	public:
		FASTINLINE ElemType& operator[](ssize_t Offset) {
			BOX_ASSERT(Offset >= 0 && Offset < ssize_t(Length), "Out of range");
			return Storage[Offset]; 
		}

		FASTINLINE const ElemType& operator[](ssize_t Offset) const {
			BOX_ASSERT(Offset >= 0 && Offset < ssize_t(Length), "Out of range");
			return Storage[Offset]; 
		}

		FASTINLINE bool IsValid(ssize_t Offset) const { return Offset >= 0 && Offset < ssize_t(Length); }

	public:
		FASTINLINE Iterator Begin() const { return Iterator(*this, 0); }
		FASTINLINE Iterator End() const { return Iterator(*this, Length); }

	protected:
		/*
			Try to hold required size.
			Finally, this array will have (m_Uses + Size) capacity in minimal.
		*/
		FASTINLINE bool Requires(size_t Size) {
			if (Length + Size > Capacity) {
				ElemType* Storage = (ElemType*) new u8[
					(Length + Size) * Multiplier * sizeof(ElemType)];

				BOX_ASSERT(Storage != nullptr, "Out of memory.");

				if (IsPodType<ElemType>) {
					::memcpy(Storage, this->Storage, sizeof(ElemType) * Length);
				}

				else {
					for (size_t i = 0; i < Length; i++) {
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

				BOX_ASSERT(Storage != nullptr, "Out of memory.");

				if (IsPodType<ElemType>) {
					::memcpy(Storage, this->Storage, sizeof(ElemType) * Length);
				}
				else {
					for (size_t i = 0; i < Length; i++) {
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
					for (size_t i = 0; i < Length; i++) {
						Storage[i].~ElemType();
					}

					this->Length = 0;
				}
				else {
					ElemType* Storage = nullptr;
					size_t Capacity = 0;
					size_t Length = 0;

					Swap(this->Storage, Storage);
					Swap(this->Capacity, Capacity);
					Swap(this->Length, Length);

					for (size_t i = 0; i < Length; i++) {
						Storage[i].~ElemType();
					}

					delete[]((u8*)Storage);
				}
			}
		}

		/*
			Remove an item (or items) from this array.
		*/
		FASTINLINE bool RemoveAt(ssize_t Offset, size_t Count = 1, bool bOptimize = true) {
			if (BOX_ENSURE(Offset >= 0 && Offset < ssize_t(Length))) {
				size_t FinalLength = size_t(Length < Count ? 0 : Length - Count);

				if (IsPodType<ElemType>) {
					::memmove(Storage + Offset, Storage + Offset + Count,
						sizeof(ElemType) * (Length - (Offset + Count)));
				}
				else {
					for (size_t i = size_t(Offset); i < FinalLength; i++) {
						Storage[i].~ElemType();

						new (Storage + i) ElemType(
							TMovable<ElemType>::Movable(Storage[i + Count])
						);
					}

					for (size_t i = FinalLength; i < Length; i++) {
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
		FASTINLINE ssize_t RemoveLike(PredicateType&& Predicate, size_t Offset = 0, bool bOptimize = true) {
			size_t Count = 0;
			size_t i = Offset;

			while (i < Length) {
				if (Predicate(Storage[i])) {
					RemoveAt(i, 1, false);
					++Count;
				}

				else ++i;
			}

			if (bOptimize)
				Optimize();

			return ssize_t(Count);
		}

		/*
			Find item index using predicate.
		*/
		template<typename PredicateType>
		FASTINLINE ssize_t Find(PredicateType&& Predicate, size_t Offset = 0) const {
			for (size_t i = Offset; i < Length; i++) {
				if (Predicate(Storage[i]))
					return ssize_t(i);
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
