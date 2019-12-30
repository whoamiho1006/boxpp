#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/containers/Node.hpp>
#include <boxpp/base/tpls/containers/Iterator.hpp>
#include <boxpp/base/systems/Debugger.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TLinkedList
	{
	public:
		typedef TNode<ElemType> NodeType;
		typedef typename TNode<ElemType>::OffsetType OffsetType;
		typedef typename TNode<ElemType>::ReverseOffsetType ReverseOffsetType;

		typedef TIterator<TLinkedList<ElemType>, false> Iterator;
		typedef TIterator<const TLinkedList<ElemType>, false> ConstIterator;

	public:
		TLinkedList()
			: First(nullptr), Last(nullptr)
		{
		}

		TLinkedList(const TLinkedList<ElemType>& Other)
			: First(nullptr), Last(nullptr)
		{
			Append(Other);
		}

		TLinkedList(TLinkedList<ElemType>&& Other)
			: First(Other.First), Last(Other.Last),
			CachedLength(Other.CachedLength)
		{
			Other.First = Other.Last = nullptr;
			Other.CachedLength = 0;
		}

		~TLinkedList()
		{
			Clear();
		}

	private:
		NodeType* First, *Last;
		u32 CachedLength;

	public:
		FASTINLINE operator bool() const { return First && Last; }
		FASTINLINE bool operator !() const { return !First || !Last; }

		FASTINLINE bool IsEmpty() const { return !First || !Last; }

		FASTINLINE bool operator ==(const TLinkedList<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TLinkedList<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE u32 GetSize() const { return CachedLength; }
		FASTINLINE NodeType* GetRaw() const { return First; }

	public:
		FASTINLINE ElemType& operator[](const OffsetType& Offset) {
			BOX_ASSERT(Offset && Offset->GetRaw(), "Invalid Node offset!");
			return *Offset->GetRaw();
		}
		FASTINLINE const ElemType& operator[](const OffsetType& Offset) const {
			BOX_ASSERT(Offset && Offset->GetRaw(), "Invalid Node offset!");
			return *Offset->GetRaw(); 
		}

	public:
		FASTINLINE bool IsValid(const OffsetType& Offset) const { return *Offset != nullptr; }
		FASTINLINE bool IsValid(const ReverseOffsetType& Offset) const { return *Offset != nullptr; }

	public:
		FASTINLINE Iterator Begin() const { return Iterator(*this, First); }
		FASTINLINE Iterator End() const { return Iterator(*this, OffsetType(nullptr)); }

	public:
		FASTINLINE TLinkedList<ElemType>& operator =(const TLinkedList<ElemType>& Other) {
			if (*this != Other) {
				this->Clear();
				Append(Other);
			}

			return *this;
		}

		FASTINLINE TLinkedList<ElemType>& operator =(TLinkedList<ElemType>&& Other) {
			if (*this != Other) {
				Swap(this->First, Other.First);
				Swap(this->Last, Other.Last);
				Swap(this->CachedLength, Other.CachedLength);
			}

			return *this;
		}

	public:
		/* Clear this linked list and release all memory. */
		FASTINLINE void Clear()
		{
			if (First && Last)
			{
				NodeType* Current = First->GetFirstEdge(),
						* Temp = nullptr;

				First = Last = nullptr;
				CachedLength = 0;

				while (Current)
				{
					Current = (Temp = Current)->GetNext();
					delete Temp;
				}
			}
		}

		/* Refresh node state. */
		FASTINLINE void Refresh() {
			First = First ? First->GetFirstEdge() : nullptr;
			Last = Last ? Last->GetLastEdge() : nullptr;
		}
		
		/* Add item and returns its first offset. */
		FASTINLINE OffsetType Add(const ElemType& Item, u32 Count = 1) {
			if (Count) {
				NodeType* Checkpoint = Last;

				if (!Last) {
					First = Last = 
						Checkpoint = new NodeType(Item);

					--Count;
					++CachedLength;
				}

				while (Count) {
					--Count;
					++CachedLength;

					Last = Last->Emplace(Item);
				}

				return Checkpoint;
			}

			return nullptr;
		}

		/* Add item and returns its first offset. */
		FASTINLINE OffsetType Add(ElemType&& Item) {
			NodeType* Checkpoint = Last;

			if (!Last) {
				First = Last =
					Checkpoint = new NodeType(Item);

				++CachedLength;
			}

			else {
				Last = Last->Emplace(Item);
				++CachedLength;
			}

			return Checkpoint;
		}

		/* Append all items from list. */
		FASTINLINE OffsetType Append(const TLinkedList<ElemType>& InList)
		{
			NodeType* Current = InList.First;
			NodeType* Checkpoint = Last;

			while (Current)
			{
				if (ElemType* ValuePtr = Current->GetRaw()) {
					if (!Checkpoint)
						Checkpoint = Add(*ValuePtr);

					else Add(*ValuePtr);
				}

				Current = Current->GetNext();
			}

			return Checkpoint;
		}

		/* Append all items from list. */
		FASTINLINE OffsetType Append(TLinkedList<ElemType>&& InList)
		{
			NodeType* Checkpoint = Last;

			Refresh();
			InList.Refresh();

			if (!Last)
			{
				Swap(First, InList.First);
				Swap(Last, InList.Last);
				Swap(CachedLength, InList.CachedLength);
			}

			else
			{
				Last->SetNext(InList.First);
				Last = InList.Last;

				CachedLength += InList.CachedLength;

				InList.First = InList.Last = nullptr;
				InList.CachedLength = 0;
			}

			return Checkpoint;
		}

		/*
			Append all items in given array into this array.
		*/
		FASTINLINE OffsetType Append(const ElemType* Items, u32 Count) {
			if (Items && Count) {
				NodeType* Checkpoint = *Add(*Items);

				--Count;
				++Items;

				while (Count) {
					Add(*Items);

					--Count;
					++Items;
				}

				return Checkpoint;
			}

			return nullptr;
		}

		/* Insert items and returns first offset.*/
		FASTINLINE OffsetType Insert(const OffsetType& Offset, const ElemType& Elem, u32 Count = 1) {
			if (Offset)
			{
				NodeType* Edge = *Offset;
				NodeType* Checkpoint = nullptr;

				if (Edge == First) {
					First = Edge =
						Checkpoint = Edge->Emplace(Elem, true);

					--Count;
					++CachedLength;
				}

				while (Edge && Count) {
					--Count;
					++CachedLength;

					Edge = Edge->Emplace(Elem, true);
					if (!Checkpoint) {
						Checkpoint = Edge;
					}
				}

				return Checkpoint;
			}

			return Add(Elem, Count);
		}

		/* Insert items and returns first offset.*/
		FASTINLINE OffsetType Insert(const OffsetType& Offset, ElemType&& Elem) {
			if (Offset)
			{
				NodeType* Edge = *Offset;
				NodeType* Checkpoint = nullptr;

				if (Edge == First) {
					First = Edge = Checkpoint = 
						Edge->Emplace(TMovable<ElemType>::Movable(Elem), true);

					++CachedLength;
				}

				return Checkpoint;
			}

			return Add(Elem);
		}

		/*
			Remove an item from this array.
		*/
		FASTINLINE bool Remove(const ElemType& Item) {
			OffsetType Offset = IndexOf(Item);

			if (Offset) {
				return this->RemoveAt(Offset, 1);
			}

			return false;
		}

		/*
			Remove all item (or items) from this array.
		*/
		FASTINLINE s32 RemoveAll(const ElemType& Item) {
			NodeType* Current = First, *Temp = nullptr;

			while (Current)
			{
				Current = (Temp = Current)->GetNext();

				if (Temp->GetRaw() &&
					!Compare(*Temp->GetRaw(), Item))
				{
					RemoveAt(Temp);
				}
			}

			return nullptr;
		}

		/* Remove items from given offset. */
		FASTINLINE bool RemoveAt(const OffsetType& Offset, u32 Count = 1) {
			if (Offset) 
			{
				NodeType* Edge = *Offset, 
						 *Temp = nullptr;

				while (Count && Edge)
				{
					--Count;
					--CachedLength;

					if (Edge == First) {
						First = Edge->GetNext();
					}

					if (Edge == Last) {
						Last = Edge->GetPrevious();
					}

					Edge = (Temp = Edge)->GetNext();
					delete Temp;
				}

				return true;
			}

			return false;
		}

	public:
		/*
			Find the index of given item.
		*/
		FASTINLINE OffsetType IndexOf(const ElemType& Item, const OffsetType& Offset = nullptr) const {
			const NodeType* Current = Offset ? *Offset : First;

			while (Current)
			{
				if (Current->GetRaw() && 
					!Compare(*Current->GetRaw(), Item)) 
				{
					return const_cast<NodeType*>(Current);
				}

				Current = Current->GetNext();
			}

			return nullptr;
		}

		/*
			Determines this array contains given item or not.
		*/
		FASTINLINE bool Contains(const ElemType& Item) const {
			return IndexOf(Item);
		}

		/* Sort this linked list. (bubble sort. so, slow) */
		FASTINLINE void Sort(bool bDescend = false) {
			NodeType* Outer = First,* Inner = nullptr;

			while (Outer)
			{
				Inner = First;

				if (Outer->GetRaw()) {

					while (Inner)
					{
						if (Inner->GetRaw())
						{
							s32 R = Compare(*Outer->GetRaw(), *Inner->GetRaw());

							if ((R < 0 && !bDescend) || 
								(R > 0 && bDescend)) 
							{
								Swap(*Outer->GetRaw(), *Inner->GetRaw());
							}
						}

						Inner = Inner->GetNext();
					}
				}

				Outer = Outer->GetNext();
			}
		}
	};

	template<typename ElemType>
	using TLinkedListIterator = typename TLinkedList<ElemType>::Iterator;

	template<typename ElemType>
	using TLinkedListConstIterator = typename TLinkedList<ElemType>::ConstIterator;

	template<typename ElemType>
	FASTINLINE TLinkedListIterator<ElemType> begin(TLinkedList<ElemType>& InList)
	{
		return InList.Begin();
	}

	template<typename ElemType>
	FASTINLINE TLinkedListIterator<ElemType> end(TLinkedList<ElemType>& InList)
	{
		return InList.End();
	}

	template<typename ElemType>
	FASTINLINE TLinkedListConstIterator<ElemType> begin(const TLinkedList<ElemType>& InList)
	{
		return InList.Begin();
	}

	template<typename ElemType>
	FASTINLINE TLinkedListConstIterator<ElemType> end(const TLinkedList<ElemType>& InList)
	{
		return InList.End();
	}

}
