#pragma once
#include <boxpp/sharedptrs/SharedCount.hpp>
#include <boxpp/sharedptrs/SharedHolder.hpp>
#include <boxpp/sharedptrs/SharedPtr.hpp>

namespace boxpp {
	/*
		Hierarchial root for broadcasted objects.
		This makes weak pointer that can get notice when the object deleted.
	*/
	template<typename SelfType, ESharedMode Mode = ESharedMode::Unsafe>
	class IBroadcastedObject
	{
	public:
		IBroadcastedObject() {
			/* Create self shareable holder with counter. */
			Holder = new sharedptr::TSharedCount<IBroadcastedObject, 
				sharedptr::TEmptyDeleter<IBroadcastedObject>>(this);
		}

		virtual ~IBroadcastedObject() {
			if (Holder) {
				sharedptr::FSharedOps<Mode>::BroadcastDeletion(Holder.GetRaw());
			}
		}

	private:
		sharedptr::FSharedHolder<Mode, false> Holder;

	public:
		/* Make weak reference to broadcasted object. */
		FASTINLINE sharedptr::TSmartProxy<SelfType> ToWeak() const {
			sharedptr::TSmartProxy<SelfType> Proxy;

			Proxy.Counter = Holder.GetRaw();
			Proxy.Object = (SelfType*)(this);

			return Proxy;
		}
	};
}