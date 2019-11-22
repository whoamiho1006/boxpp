#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/containers/SingleNode.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TStack
	{
	public:
		typedef TSingleNode<ElemType> NodeType;

	public:
		TStack()
			: First(nullptr), CachedLength(0)
		{
		}

		TStack(const TStack<ElemType>& Other)
			: First(nullptr), CachedLength(0)
		{
			NodeType* Current = const_cast<NodeType*>(Other.First);
			NodeType* Cursor = nullptr;

			while (Current)
			{
				if (Current->GetRaw())
				{
					if (Cursor)
					{
						Cursor->SetNext(new NodeType(*Current->GetRaw());
						Cursor = Cursor->GetNext();
					}

					else
					{
						First = Cursor 
							= new NodeType(*Current->GetRaw());
					}
				}

				Current = Current->GetNext();
			}
		}

		TStack(TStack<ElemType>&& Other)
			: First(Other.First), CachedLength(Other.CachedLength)
		{
			Other.First = nullptr;
			Other.CachedLength = 0;
		}

		~TStack()
		{
			Clear();
		}

	private:
		NodeType* First;
		u32 CachedLength;

	public:
		FASTINLINE operator bool() const { return First; }
		FASTINLINE bool operator !() const { return !First; }

		FASTINLINE bool IsEmpty() const { return !First; }

		FASTINLINE bool operator ==(const TStack<ElemType>& Other) const { return this == &Other; }
		FASTINLINE bool operator !=(const TStack<ElemType>& Other) const { return this != &Other; }

	public:
		FASTINLINE u32 GetSize() const { return CachedLength; }
		FASTINLINE NodeType* GetRaw() const { return First; }

	public:
		FASTINLINE TQueue<ElemType>& operator =(const TQueue<ElemType>& Other) {
			if (*this != Other) {
				this->Clear();

				NodeType* Current = const_cast<NodeType*>(Other.First);
				NodeType* Cursor = nullptr;

				while (Current)
				{
					if (Current->GetRaw())
					{
						if (Cursor)
						{
							Cursor->SetNext(new NodeType(*Current->GetRaw());
							Cursor = Cursor->GetNext();
						}

						else
						{
							First = Cursor
								= new NodeType(*Current->GetRaw());
						}
					}

					Current = Current->GetNext();
				}
			}

			return *this;
		}

		FASTINLINE TQueue<ElemType>& operator =(TQueue<ElemType>&& Other) {
			if (*this != Other) {
				Swap(this->First, Other.First);
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
					*Temp = nullptr;

				First = nullptr;
				CachedLength = 0;

				while (Current)
				{
					Current = (Temp = Current)->GetNext();
					delete Temp;
				}
			}
		}

		FASTINLINE void Push(const ElemType& Item)
		{
			if (!First)
				First = new NodeType(Item);

			else
			{
				NodeType* New = new NodeType(Item);

				New->SetNext(First);
				First = New;
			}

			++CachedLength;
		}

		FASTINLINE void Push(ElemType&& Item)
		{
			if (!First)
				First = new NodeType(TMovable<ElemType>::Movable(Item));

			else
			{
				NodeType* New = new NodeType(
					TMovable<ElemType>::Movable(Item));

				New->SetNext(First);
				First = New;
			}

			++CachedLength;
		}

		FASTINLINE bool Pop(ElemType& Item)
		{
			if (First)
			{
				NodeType* Temp = First;
				First = First->GetNext();

				--CachedLength;

				if (Temp->GetRaw()) {
					Item = TMovable<ElemType>::Movable(*Temp->GetRaw());
					delete Temp;
					return true;
				}

				delete Temp;
			}

			return false;
		}

		FASTINLINE ElemType* Peek() const
		{
			return First ? First->GetRaw() : nullptr;
		}
	};

}