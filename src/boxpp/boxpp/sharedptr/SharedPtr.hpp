#ifndef __BOXPP_SHAREDPTR_SHAREDPTR_HPP__
#define __BOXPP_SHAREDPTR_SHAREDPTR_HPP__

#include <boxpp/sharedptr/SharedCount.hpp>
#include <boxpp/sharedptr/SharedHolder.hpp>

#include <boxpp/utils/TypeCompares.hpp>
#include <boxpp/utils/Functionality.hpp>

namespace boxpp {
	
	namespace sharedptr {
		template<typename ObjectType>
		struct TSmartProxy
		{
			ISharedCount* Counter;
			ObjectType* Object;
		};

		template<typename ObjectType, ESharedMode Mode, bool bHoldStrong>
		class TSmartPtr
		{
		public:
			template<typename, ESharedMode, bool> friend class TSmartPtr;
			typedef TSmartPtr<ObjectType, Mode, bHoldStrong> SelfType;
			typedef FSharedHolder<Mode, bHoldStrong> HolderType;

		public:
			TSmartPtr(nullptr_t = nullptr) { }
			TSmartPtr(const SelfType& Other) : Holder(Other.Holder) { }
			TSmartPtr(SelfType&& Other) : Holder(TMovable<HolderType>::Movable(Other.Holder)) { }

			/* up-casting supports */
			template<typename OtherType, ESharedMode OtherMode, bool OtherHolds, 
				typename = EnableIf<IsDerivedType<ObjectType, OtherType>>>
			TSmartPtr(const TSmartPtr<OtherType, OtherMode, OtherHolds>& Other)
				: Holder(Other.GetRaw()), Object(dynamic_cast<ObjectType*>(Other.Object))
			{
			}

		public:
			TSmartPtr(const TSmartProxy<ObjectType>& Proxy)
				: Holder(Proxy.Counter), Object(Proxy.Object)
			{
			}

		public:
			FASTINLINE operator bool() const { return Holder; }
			FASTINLINE bool operator !() const { return !Holder; }

			/* Get raw pointer. */
			FASTINLINE ObjectType* GetRaw() const { return Holder ? Object : nullptr; }
			
		public:
			FASTINLINE ObjectType* operator ->() const { return GetRaw(); }
			FASTINLINE ObjectType& operator *() const { return *GetRaw(); }

		public:
			FASTINLINE bool operator ==(const SelfType& Other) const { return Holder.GetRaw() == Other.Holder.GetRaw(); }
			FASTINLINE bool operator !=(const SelfType& Other) const { return Holder.GetRaw() != Other.Holder.GetRaw(); }

		public:
			FASTINLINE SelfType& operator =(nullptr_t) { Holder = nullptr; return *this; }
			FASTINLINE SelfType& operator =(const TSmartProxy<ObjectType>& Proxy) {
				Holder = Proxy.Counter;
				Object = Proxy.Object;
				return *this;
			}

			FASTINLINE SelfType& operator =(const SelfType& Other) {
				Holder = Other.Holder;
				Object = Other.Object;
				return *this;
			}

			FASTINLINE SelfType& operator =(SelfType&& Other) {
				Holder = TMovable<HolderType>::Movable(Other.Holder);
				Swap(Object, Other.Object);
				return *this;
			}

			/* up-casting supports */
			template<typename OtherType, ESharedMode OtherMode, bool OtherHolds,
				typename = EnableIf<IsDerivedType<ObjectType, OtherType>>>
			FASTINLINE SelfType& operator =(const TSmartPtr<OtherType, OtherMode, OtherHolds>& Other) {
				Holder = Other.Holder.GetRaw();
				Object = Other.Object;
				return *this;
			}

			/* up-casting supports */
			template<typename OtherType, typename = EnableIf<IsDerivedType<ObjectType, OtherType>>>
			FASTINLINE SelfType& operator =(const TSmartProxy<OtherType>& Other) {
				Holder = Other.Counter;
				Object = Other.Object;
				return *this;
			}

		protected:
			HolderType Holder;
			ObjectType* Object;
		};
	}

	template<typename ObjectType, ESharedMode Mode = ESharedMode::Unsafe>
	using TSharedPtr = sharedptr::TSmartPtr<ObjectType, Mode, true>;

	template<typename ObjectType, ESharedMode Mode = ESharedMode::Unsafe>
	using TWeakPtr = sharedptr::TSmartPtr<ObjectType, Mode, false>;

	template<typename ObjectType>
	sharedptr::TSmartProxy<ObjectType> MakeShared(ObjectType* Object)
	{
		sharedptr::TSmartProxy<ObjectType> Proxy;

		Proxy.Counter = new sharedptr::TSharedCount<ObjectType>(Object);
		Proxy.Object = Object;

		return Proxy;
	}

	template<typename ObjectType, typename DeleterType>
	sharedptr::TSmartProxy<ObjectType> MakeShared(ObjectType* Object, DeleterType&& Deleter)
	{
		sharedptr::TSmartProxy<ObjectType> Proxy;

		Proxy.Counter = new sharedptr::TSharedCount<ObjectType>(Object, Deleter);
		Proxy.Object = Object;

		return Proxy;
	}

}

#endif