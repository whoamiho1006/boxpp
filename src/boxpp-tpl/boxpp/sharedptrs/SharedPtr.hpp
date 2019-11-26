#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/Movable.hpp>
#include <boxpp/traits/Forward.hpp>
#include <boxpp/traits/EnableIf.hpp>
#include <boxpp/traits/IsDerivedType.hpp>

#include <boxpp/sharedptrs/SharedCount.hpp>
#include <boxpp/sharedptrs/SharedHolder.hpp>

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
			TSmartPtr(const SelfType& Other) : Holder(Other.Holder), Object(Other.Object) { }
			TSmartPtr(SelfType&& Other) 
				: Holder(TMovable<HolderType>::Movable(Other.Holder)), Object(nullptr)
			{
				Swap(Object, Other.Object);
			}

			/* up/down casting supports */
			template<typename OtherType, ESharedMode OtherMode, bool OtherHolds,
				typename = EnableIf<IsDerivedType<ObjectType, OtherType> || IsDerivedType<OtherType, ObjectType>>>
				TSmartPtr(const TSmartPtr<OtherType, OtherMode, OtherHolds>& Other)
				: Holder(Other.Holder.GetRaw()), Object(dynamic_cast<ObjectType*>(Other.Object))
			{
			}

		public:
			TSmartPtr(const TSmartProxy<ObjectType>& Proxy)
				: Holder(Proxy.Counter), Object(Proxy.Object)
			{
			}

			/* up/down casting supports */
			template<typename OtherType>
			TSmartPtr(const TSmartProxy<OtherType>& Proxy)
				: Holder(Proxy.Counter), Object(Proxy.Object)
			{
				static_assert(IsDerivedType<ObjectType, OtherType> ||
					IsDerivedType<OtherType, ObjectType>, "");
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

			/* up/down casting supports */
			template<typename OtherType, ESharedMode OtherMode, bool OtherHolds,
				typename = EnableIf<IsDerivedType<ObjectType, OtherType> || IsDerivedType<OtherType, ObjectType>>>
			FASTINLINE SelfType& operator =(const TSmartPtr<OtherType, OtherMode, OtherHolds>& Other) {
				Holder = Other.Holder.GetRaw();
				Object = Other.Object;
				return *this;
			}

			/* up/down casting supports */
			template<typename OtherType, typename = EnableIf<
				IsDerivedType<ObjectType, OtherType> || 
				IsDerivedType<OtherType, ObjectType>>>
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

		Proxy.Counter = new sharedptr::TSharedCount<ObjectType>(Object, Forward<DeleterType>(Deleter));
		Proxy.Object = Object;

		return Proxy;
	}
}