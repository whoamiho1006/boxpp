#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/containers/EdgeBase.hpp>

namespace boxpp
{
	template<typename ElemType>
	class TNode : public TEdgeBase<ElemType>
	{
	public:
		TNode() : Previous(nullptr), Next(nullptr) { }
		TNode(const ElemType& Item) : TEdgeBase<ElemType>(Item), Previous(nullptr), Next(nullptr) { }
		TNode(ElemType&& Item) : TEdgeBase<ElemType>(TMovable<ElemType>::Movable(Item)), Previous(nullptr), Next(nullptr) { }
		virtual ~TNode() { Unlink(); }

	private:
		TNode<ElemType>* Previous;
		TNode<ElemType>* Next;

	public:
		FASTINLINE TNode<ElemType>* GetPrevious() const { return Previous; }
		FASTINLINE TNode<ElemType>* GetNext() const { return Next; }

	public:
		FASTINLINE bool IsFirstEdge() const { return !Previous; }
		FASTINLINE bool IsLastEdge() const { return !Next; }

	public:
		/* Get first edge. WARNING: This method is slow! */
		FASTINLINE TNode<ElemType>* GetFirstEdge() const {
			TNode<ElemType>* Edge = const_cast<TNode<ElemType>*>(this);

			while (Edge->Previous) 
				Edge = Edge->Previous;

			return Edge;
		}

		/* Get last edge. WARNING: This method is slow! */
		FASTINLINE TNode<ElemType>* GetLastEdge() const {
			TNode<ElemType>* Edge = const_cast<TNode<ElemType>*>(this);

			while (Edge->Next)
				Edge = Edge->Next;

			return Edge;
		}

		/* Determines given edge is compositing edge or not. (SLOW!) */
		FASTINLINE bool IsCompositingEdge(TNode<ElemType>* Edge) const {
			TNode<ElemType>* Current = const_cast<TNode<ElemType>*>(this);

			while (Current && Current != Edge) {
				Current = Current->Next;
			}

			if (Current == Edge)
				return true;

			// Rewind to this edge.
			Current = this;

			while (Current && Current != Edge) {
				Current = Current->Previous;
			}

			if (Current == Edge)
				return true;

			return false;
		}

		/* Determines this edge-list has loop or not. (SLOW) */
		FASTINLINE TNode<ElemType>* DetectLoop(bool bForward = true) const {
			TNode<ElemType>* Alpha = const_cast<TNode<ElemType>*>(this), 
				*Beta = const_cast<TNode<ElemType>*>(this);

			if (bForward) {
				while (Alpha && Beta && Beta->Next) {
					Alpha = Alpha->Next;
					Beta = Beta->Next->Next;

					if (Alpha == Beta)
						return Alpha;
				}
			}

			else {
				while (Alpha && Beta && Beta->Previous) {
					Alpha = Alpha->Previous;
					Beta = Beta->Previous->Previous;

					if (Alpha == Beta)
						return Alpha;
				}
			}

			return nullptr;
		}

	private:
		FASTINLINE TNode<ElemType>* EmplaceInternal(TNode<ElemType>* Edge, bool bPrevious = false) {
			if (bPrevious) {
				Edge->Next = this;
				Edge->Previous = Previous;

				if (Previous) {
					Previous->Next = Edge;
				}

				Previous = Edge;
			}

			else {
				Edge->Next = Next;
				Edge->Previous = this;

				if (Next) {
					Next->Previous = Edge;
				}

				Next = Edge;
			}

			return Edge;
		}

	public:
		/*
			Unlink this node and returns the valid edge which was PREV or NEXT.
		*/
		FASTINLINE TNode<ElemType>* Unlink() {
			TNode<ElemType>* RetEdge = nullptr;

			if (Previous) 
				Previous->Next = Next;

			if (Next) 
				Next->Previous = Previous;
			
			RetEdge = Previous ? Previous : Next;
			Previous = Next = nullptr;

			return RetEdge;
		}

		/* Unlink next and returns previous next. */
		FASTINLINE TNode<ElemType>* SetNext(TNode<ElemType>* NewNext)
		{
			TNode<ElemType>* RetVal = Split(false);

			NewNext->Previous = this;
			Next = NewNext;

			return RetVal;
		}


		/* Unlink next and returns previous next. */
		FASTINLINE TNode<ElemType>* SetPrevious(TNode<ElemType>* NewPrevious)
		{
			TNode<ElemType>* RetVal = Split(true);

			NewPrevious->Next = this;
			Previous = NewPrevious;

			return RetVal;
		}

		/*
			Emplace entire edges that linked in given edge.
			returns first node which appended for bPrevious == true.

			WARNING: This method is SLOW when so much edges are being on given edge!
		*/
		FASTINLINE TNode<ElemType>* Emplace(TNode<ElemType>* Edge, bool bPrevious = false)
		{
			TNode<ElemType>* First = Edge ? Edge->GetFirstEdge() : nullptr;
			TNode<ElemType>* Last = Edge ? Edge->GetLastEdge() : nullptr;

			if (First && Last) {
				if (bPrevious) {
					First->Previous = Previous;
					Last->Next = this;

					if (Previous) {
						Previous->Next = First;
					}

					Previous = Last;
					return First;
				}

				else {
					First->Previous = this;
					Last->Next = Next;

					if (Next) {
						Next->Previous = Last;
					}

					Next = First;
					return Last;
				}
			}

			return nullptr;
		}

	public:
		/*
			Insert an edge that filled with given value and
			returns new edge.
		*/
		FASTINLINE TNode<ElemType>* Emplace(const ElemType& Value, bool bPrevious = false) {
			if (TNode<ElemType>* Edge = new TNode<ElemType>(Value)) {
				return EmplaceInternal(Edge, bPrevious);
			}

			return nullptr;
		}

		/*
			Insert an edge that filled with given value and
			returns new edge.
		*/
		FASTINLINE TNode<ElemType>* Emplace(ElemType&& Value, bool bPrevious = false) {
			if (TNode<ElemType>* Edge = new TNode<ElemType>(TMovable<ElemType>::Movable(Value))) {
				return EmplaceInternal(Edge, bPrevious);
			}

			return nullptr;
		}

		/* Split next edge or previous edge and return its pointer. */
		FASTINLINE TNode<ElemType>* Split(bool bPrevious = false) {
			TNode<ElemType>* Edge = bPrevious ? Previous : Next;

			if (Edge)
			{
				if (bPrevious) {
					if (Edge->Next == this)
						Edge->Next = nullptr;

					Previous = nullptr;
				}
				else {
					if (Edge->Previous == this)
						Edge->Previous = nullptr;

					Next = nullptr;
				}
			}

			return Edge;
		}

	public:
		/* Offset-type for traveling nodes. */
		class OffsetType
		{
		public:
			OffsetType(nullptr_t = nullptr) : Edge(nullptr) { }
			OffsetType(TNode<ElemType>* Edge) : Edge(Edge) { }

			OffsetType(const OffsetType& Other) : Edge(Other.Edge) { };
			OffsetType(OffsetType&& Other) : Edge(Other.Edge) { Other.Edge = nullptr; };

		private:
			TNode<ElemType>* Edge;

		public:
			FASTINLINE operator bool() const { return Edge; }
			FASTINLINE bool operator !() const { return !Edge; }

			FASTINLINE TNode<ElemType>* operator *() const { return Edge; }
			FASTINLINE TNode<ElemType>* operator ->() const { return Edge; }

			FASTINLINE bool operator ==(const OffsetType& Right) const { return Edge == Right.Edge; }
			FASTINLINE bool operator !=(const OffsetType& Right) const { return Edge != Right.Edge; }

			FASTINLINE OffsetType& operator =(const OffsetType& Other) { Edge = Other.Edge; return *this; }
			FASTINLINE OffsetType& operator =(OffsetType&& Other) { Edge = Other.Edge; return *this; }

		public:
			FASTINLINE OffsetType& operator ++() {
				if (*this) { Edge = Edge->GetNext(); }
				return *this;
			}

			FASTINLINE OffsetType& operator --() {
				if (*this) { Edge = Edge->GetPrevious(); }
				return *this;
			}

			FASTINLINE OffsetType operator ++(int) {
				OffsetType Clone = Edge;
				if (*this) { Edge = Edge->GetNext(); }
				return Clone;
			}

			FASTINLINE OffsetType operator --(int) {
				OffsetType Clone = Edge;
				if (*this) { Edge = Edge->GetPrevious(); }
				return Clone;
			}
		};

	public:
		/* Offset-type for traveling nodes. */
		class ReverseOffsetType
		{
		public:
			ReverseOffsetType(nullptr_t = nullptr) : Edge(nullptr) { }
			ReverseOffsetType(TNode<ElemType>* Edge) : Edge(Edge) { }

			ReverseOffsetType(const ReverseOffsetType& Other) : Edge(Other.Edge) { };
			ReverseOffsetType(ReverseOffsetType&& Other) : Edge(Other.Edge) { Other.Edge = nullptr; };

		private:
			TNode<ElemType>* Edge;

		public:
			FASTINLINE operator bool() const { return Edge; }
			FASTINLINE bool operator !() const { return !Edge; }

			FASTINLINE TNode<ElemType>* operator *() const { return Edge; }
			FASTINLINE TNode<ElemType>* operator ->() const { return Edge; }

			FASTINLINE bool operator ==(const ReverseOffsetType& Right) const { return Edge == Right.Edge; }
			FASTINLINE bool operator !=(const ReverseOffsetType& Right) const { return Edge != Right.Edge; }

			FASTINLINE ReverseOffsetType& operator =(const ReverseOffsetType& Other) { Edge = Other.Edge; return *this; }
			FASTINLINE ReverseOffsetType& operator =(ReverseOffsetType&& Other) { Edge = Other.Edge; return *this; }

		public:
			FASTINLINE ReverseOffsetType& operator ++() {
				if (*this) { Edge = Edge->GetPrevious(); }
				return *this;
			}

			FASTINLINE ReverseOffsetType& operator --() {
				if (*this) { Edge = Edge->GetNext(); }
				return *this;
			}

			FASTINLINE ReverseOffsetType operator ++(int) {
				OffsetType Clone = Edge;
				if (*this) { Edge = Edge->GetPrevious(); }
				return Clone;
			}

			FASTINLINE ReverseOffsetType operator --(int) {
				OffsetType Clone = Edge;
				if (*this) { Edge = Edge->GetNext(); }
				return Clone;
			}
		};
	};

	template<typename ElemType>
	FASTINLINE typename TNode<ElemType>::OffsetType begin(TNode<ElemType>* Node) { return Node; }

	template<typename ElemType>
	FASTINLINE typename TNode<ElemType>::OffsetType end(TNode<ElemType>* Node) { return nullptr; }
	
	template<typename ElemType>
	FASTINLINE typename TNode<ElemType>::ReverseOffsetType rbegin(TNode<ElemType>* Node) { return Node; }

	template<typename ElemType>
	FASTINLINE typename TNode<ElemType>::ReverseOffsetType rend(TNode<ElemType>* Node) { return nullptr; }
}
