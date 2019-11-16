#ifndef __BOXPP_CONTAINERS_NODEBASE_HPP__
#define __BOXPP_CONTAINERS_NODEBASE_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/Movable.hpp>
#include <boxpp/utils/Instrusive.hpp>

namespace boxpp
{
	/*	Generic Edge's base.
		Note: All edges are handled as pointer. so they have no copy/move operations.
	*/
	template<typename ElemType>
	class TEdgeBase
	{
	public:
		TEdgeBase()
		{
		}

		TEdgeBase(const ElemType& Item)
		{
			this->Item.ConstructFrom(Item);
		}

		TEdgeBase(ElemType&& Item)
		{
			this->Item.ConstructFrom(TMovable<ElemType>::Movable(Item));
		}

		virtual ~TEdgeBase() { }

	private:
		TInstrusive<ElemType> Item;
		
	public:
		/* Get raw pointer. */
		FASTINLINE ElemType* GetRaw() const {
			return Item.GetRaw();
		}

		/* Set item. */
		FASTINLINE void Set(const ElemType& Item) {
			if (this->Item) {
				this->Item.Destruct();
			}

			this->Item.ConstructFrom(Item);
		}

		/* Set item. */
		FASTINLINE void Set(ElemType&& Item) {
			if (this->Item) {
				this->Item.Destruct();
			}

			this->Item.ConstructFrom(TMovable<ElemType>::Movable(Item));
		}
	};

}

#endif // !__BOXPP_CONTAINERS_NODEBASE_HPP__