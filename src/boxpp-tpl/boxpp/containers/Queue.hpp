#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/containers/SingleNode.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TQueue
	{
	public:
		typedef TSingleNode<ElemType> NodeType;

	public:
		TQueue()
			: First(nullptr), CachedLast(nullptr), CachedLength(0)
		{
		}

		TQueue(const TQueue<ElemType>& Other)
			: First(nullptr), CachedLast(nullptr)
		{
			NodeType* Current = const_cast<NodeType*>(Other.First);

			while (Current)
			{
				if (Current->GetRaw())
					Enqueue(*Current->GetRaw());

				Current = Current->GetNext();
			}
		}

		TQueue(TQueue<ElemType>&& Other)
			: First(Other.First), CachedLast(Other.CachedLast),
			CachedLength(Other.CachedLength)
		{
			Other.First = Other.CachedLast = nullptr;
			Other.CachedLength = 0;
		}

		~TQueue()
		{
			Clear();
		}

	private:
		NodeType* First, *CachedLast;
		u32 CachedLength;

	public:
		FASTINLINE operator bool() const { return First; }
		FASTINLINE bool operator !() const { return !First; }

		FASTINLINE bool IsEmpty() const { return !First; }

		FASTINLINE bool operator ==(const TQueue<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TQueue<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE u32 GetSize() const { return CachedLength; }
		FASTINLINE NodeType* GetRaw() const { return First; }

	public:
		FASTINLINE TQueue<ElemType>& operator =(const TQueue<ElemType>& Other) {
			if (*this != Other) {
				this->Clear();

				NodeType* Current = const_cast<NodeType*>(Other.First);

				while (Current)
				{
					if (Current->GetRaw())
						Enqueue(*Current->GetRaw());

					Current = Current->GetNext();
				}
			}

			return *this;
		}

		FASTINLINE TQueue<ElemType>& operator =(TQueue<ElemType>&& Other) {
			if (*this != Other) {
				Swap(this->First, Other.First);
				Swap(this->CachedLast, Other.CachedLast);
				Swap(this->CachedLength, Other.CachedLength);
			}

			return *this;
		}

	public:
		/* Clear this linked list and release all memory. */
		FASTINLINE void Clear()
		{
			if (First)
			{
				NodeType* Current = First,
						* Temp = nullptr;

				First = CachedLast = nullptr;
				CachedLength = 0;

				while (Current)
				{
					Current = (Temp = Current)->GetNext();
					delete Temp;
				}
			}
		}

		FASTINLINE void Enqueue(const ElemType& Item)
		{
			if (!First || !CachedLast)
				First = CachedLast = new NodeType(Item);

			else if (CachedLast)
			{
				CachedLast->SetNext(new NodeType(Item));
				CachedLast = CachedLast->GetLastEdge();
			}

			++CachedLength;
		}

		FASTINLINE bool Dequeue(ElemType& OutItem)
		{
			if (First)
			{
				NodeType* Temp = First;
				First = First->GetNext();
				--CachedLength;

				if (CachedLast == Temp)
					CachedLast = First;


				if (Temp->GetRaw()) {
					Swap(*Temp->GetRaw(), OutItem);

					delete Temp;
					return true;
				}

				delete Temp;
				return false;
			}

			return false;
		}

		FASTINLINE bool Dequeue()
		{
			if (First)
			{
				NodeType* Temp = First;
				First = First->GetNext();
				--CachedLength;

				if (CachedLast == Temp)
					CachedLast = First;

				if (Temp->GetRaw()) {
					delete Temp;
					return true;
				}

				delete Temp;
				return false;
			}

			return false;
		}

		FASTINLINE ElemType* Peek() const 
		{
			return First ? First->GetRaw() : nullptr;
		}
	};
}
