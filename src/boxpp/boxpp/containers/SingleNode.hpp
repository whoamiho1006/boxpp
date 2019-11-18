#ifndef __BOXPP_CONTAINERS_SINGLENODE_HPP__
#define __BOXPP_CONTAINERS_SINGLENODE_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/containers/EdgeBase.hpp>

namespace boxpp
{

	template<typename ElemType>
	class TSingleNode : public TEdgeBase<ElemType>
	{
	public:
		TSingleNode() : Next(nullptr) { }
		TSingleNode(const ElemType& Item) : TEdgeBase<ElemType>(Item), Next(nullptr) { }
		TSingleNode(ElemType&& Item) : TEdgeBase<ElemType>(Item), Next(nullptr) { }

	private:
		TSingleNode<ElemType>* Next;

	public:
		FASTINLINE TSingleNode<ElemType>* GetNext() const { return Next; }

	public:
		FASTINLINE bool IsLastEdge() const { return !Next; }

	public:
		/* Get last edge. WARNING: This method is slow! */
		FASTINLINE TSingleNode<ElemType>* GetLastEdge() const {
			TSingleNode<ElemType>* Edge = const_cast<TSingleNode<ElemType>*>(this);

			while (Edge->Next)
				Edge = Edge->Next;

			return Edge;
		}

		/* Determines given edge is compositing edge or not. (SLOW!) */
		FASTINLINE bool IsCompositingEdge(TSingleNode<ElemType>* Edge) const {
			TSingleNode<ElemType>* Current = const_cast<TSingleNode<ElemType>*>(this);

			while (Current && Current != Edge) {
				Current = Current->Next;
			}

			if (Current == Edge)
				return true;

			return false;
		}

		/* Determines this edge-list has loop or not. (SLOW) */
		FASTINLINE TSingleNode<ElemType>* DetectLoop() const {
			TSingleNode<ElemType>* Alpha = this, *Beta = this;

			while (Alpha && Beta && Beta->Next) {
				Alpha = Alpha->Next;
				Beta = Beta->Next->Next;

				if (Alpha == Beta)
					return Alpha;
			}

			return nullptr;
		}

	public:
		/*
			Unlink this node and returns the valid edge which was NEXT.
		*/
		FASTINLINE TSingleNode<ElemType>* Unlink() {
			return SetNext(nullptr);
		}

		/* Unlink next and returns previous next. */
		FASTINLINE TSingleNode<ElemType>* SetNext(TSingleNode<ElemType>* NewNext)
		{
			TSingleNode<ElemType>* RetEdge = Next;

			Next = NewNext;

			return RetEdge;
		}

	public:
		/* Offset-type for traveling nodes. */
		class OffsetType
		{
		public:
			OffsetType(nullptr_t = nullptr) : Edge(nullptr) { }
			OffsetType(TSingleNode<ElemType>* Edge) : Edge(Edge) { }

			OffsetType(const OffsetType& Other) : Edge(Other.Edge) { };
			OffsetType(OffsetType&& Other) : Edge(Other.Edge) { Other.Edge = nullptr; };

		private:
			TSingleNode<ElemType>* Edge;

		public:
			FASTINLINE operator bool() const { return Edge; }
			FASTINLINE bool operator !() const { return !Edge; }

			FASTINLINE TSingleNode<ElemType>* operator *() const { return Edge; }
			FASTINLINE TSingleNode<ElemType>* operator ->() const { return Edge; }

			FASTINLINE bool operator ==(const OffsetType& Right) const { return Edge == Right.Edge; }
			FASTINLINE bool operator !=(const OffsetType& Right) const { return Edge != Right.Edge; }

			FASTINLINE OffsetType& operator =(const OffsetType& Other) { Edge = Other.Edge; return *this; }
			FASTINLINE OffsetType& operator =(OffsetType&& Other) { Edge = Other.Edge; return *this; }

		public:
			FASTINLINE OffsetType& operator ++() {
				if (*this) { Edge = Edge->GetNext(); }
				return *this;
			}

			FASTINLINE OffsetType operator ++(int) {
				OffsetType Clone = Edge;
				if (*this) { Edge = Edge->GetNext(); }
				return Clone;
			}
		};
	};

}

#endif // !__BOXPP_CONTAINERS_SINGLENODE_HPP__