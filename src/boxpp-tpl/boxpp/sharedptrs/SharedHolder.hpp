#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/Movable.hpp>

#include <boxpp/sharedptrs/SharedCount.hpp>

namespace boxpp {
	namespace sharedptr {
		enum class EHolderPolicy
		{
			/* Default object policy. */
			Default = 0,

			/* Broadcasted objects are self-shared. so, always weak. */
			Broadcasts
		};

		template<ESharedMode Mode, bool bHoldStrong>
		class FSharedHolder
		{
		public:
			template<ESharedMode, bool> friend class FSharedHolder;
			typedef FSharedHolder<Mode, bHoldStrong> SelfType;

		public:
			FSharedHolder(nullptr_t = nullptr)
				: Counter(nullptr)
			{
			}

			FSharedHolder(ISharedCount* Counter)
				: Counter(Counter)
			{
				Grab();
			}

			FSharedHolder(const SelfType& Other)
				: Counter(Other.Counter)
			{
				Grab();
			}

			FSharedHolder(SelfType&& Other)
				: Counter(Other.Counter)
			{
				Other.Counter = nullptr;
			}

			~FSharedHolder() { Reset(); }

		public:
			FASTINLINE operator bool() const { return Counter && Counter->Validity; }
			FASTINLINE bool operator !() const { return !Counter || !Counter->Validity; }

			/* Get raw pointer. */
			FASTINLINE ISharedCount* GetRaw() const { return Counter; }

		public:
			/* Reset this holder to nullptr. */
			FASTINLINE void Reset() {
				if (Counter) {
					if (bHoldStrong && FSharedOps<Mode>::SubStrongs(Counter)) {
						FSharedOps<Mode>::DeleteObject(Counter);
					}

					if (FSharedOps<Mode>::SubWeaks(Counter)) {
						FSharedOps<Mode>::Delete(Counter);
					}

					Counter = nullptr;
				}
			}

		private:
			FASTINLINE void Grab() {
				if (Counter) {
					FSharedOps<Mode>::AddWeaks(Counter);

					if (bHoldStrong) {
						FSharedOps<Mode>::AddStrongs(Counter);
					}
				}
			}

		public:
			FASTINLINE SelfType& operator =(nullptr_t) {
				Reset();
				return *this;
			}

			FASTINLINE SelfType& operator =(const SelfType& Other) {
				if (Counter != Other.Counter) {
					Reset();
					Counter = Other.Counter;
					Grab();
				}

				return *this;
			}

			FASTINLINE SelfType& operator =(ISharedCount* Counter) {
				if (this->Counter != Counter) {
					Reset();
					this->Counter = Counter;
					Grab();
				}

				return *this;
			}

			FASTINLINE SelfType& operator =(SelfType&& Other) {
				if (Counter != Other.Counter) {
					Swap(Counter, Other.Counter);
				}

				return *this;
			}

		private:
			ISharedCount* Counter;
		};
	}
}
