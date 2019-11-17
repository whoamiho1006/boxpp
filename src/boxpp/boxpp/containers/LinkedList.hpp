#ifndef __BOXPP_CONTAINERS_LINKEDLIST_HPP__
#define __BOXPP_CONTAINERS_LINKEDLIST_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/containers/Node.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TLinkedList
	{
	public:
		typedef TNode<ElemType> NodeType;
		typedef typename TNode<ElemType>::OffsetType OffsetType;
		typedef typename TNode<ElemType>::ReverseOffsetType ReverseOffsetType;

		typedef TIterator<TLinkedList<ElemType>> Iterator;
		typedef TIterator<const TLinkedList<ElemType>> ConstIterator;

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
			: First(Other.First), Last(Other.Last)
		{
			Other.First = Other.Last = nullptr;
		}

		~TLinkedList()
		{
			Clear();
		}

	private:
		NodeType* First, *Last;
		u32 Length;

	public:
		FASTINLINE operator bool() const { return First && Last; }
		FASTINLINE bool operator !() const { return !First || !Last; }

		FASTINLINE bool operator ==(const TArrayBase<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TArrayBase<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE u32 GetSize() const { return Length; }
		FASTINLINE NodeType* GetRaw() const { return First; }

	public:
		FASTINLINE bool IsValid(const OffsetType& Offset) const { return *Offset != nullptr; }
		FASTINLINE bool IsValid(const ReverseOffsetType& Offset) const { return *Offset != nullptr; }

	public:
		FASTINLINE Iterator Begin() const { return Iterator(*this, First); }
		FASTINLINE Iterator End() const { return Iterator(*this, nullptr); }

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
				Swap(this->Length, Other.Length);
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
				Length = 0;

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
					++Length;
				}

				while (Count) {
					--Count;
					++Length;

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

				++Length;
			}

			else {
				Last = Last->Emplace(Item);
				++Length;
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
						Checkpoint = Add(*CurrentValuePtr);

					else Add(*CurrentValuePtr);
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
				Swap(Length, InList.Length);
			}

			else
			{
				Last->SetNext(InList.First);
				Last = InList.Last;

				Length += InList.Length;

				InList.First = InList.Last = nullptr;
				InList.Length = 0;
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
					++Length;
				}

				while (Edge && Count) {
					--Count;
					++Length;

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

					--Count;
					++Length;
				}

				while (Edge && Count) {
					--Count;
					++Length;

					Edge = Edge->Emplace(TMovable<ElemType>::Movable(Elem), true);
					if (!Checkpoint) {
						Checkpoint = Edge;
					}
				}

				return Checkpoint;
			}

			return Add(Elem, Count);
		}

		/*
			Remove an item from this array.
		*/
		FASTINLINE bool Remove(const ElemType& Item, bool bOptimize = true) {
			OffsetType Offset = IndexOf(Item);

			if (Offset) {
				return this->RemoveAt(Offset, 1, bOptimize);
			}

			return false;
		}

		/*
			Remove all item (or items) from this array.
		*/
		FASTINLINE s32 RemoveAll(const ElemType& Item, bool bOptimize = true) {
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
					--Length;

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
		FASTINLINE OffsetType IndexOf(const ElemType& Item, const NodeType& Offset = nullptr) const {
			NodeType* Current = Offset ? *Offset : First;

			while (Current)
			{
				if (Current->GetRaw() && 
					!Compare(*Current->GetRaw(), Item)) 
				{
					return Current;
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
			NodeType* Outer = First, Inner = nullptr;

			while (Outer)
			{
				Inner = First;

				if (Outer->GetRaw()) {

					while (Inner)
					{
						if (Inner->GetRaw())
						{
							s32 R = Compare(*Outer->GetRaw(), *Inner->GetRaw());

							if ((R > 0 && !bDescend) || 
								(R < 0 && bDescend)) 
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

}

#endif