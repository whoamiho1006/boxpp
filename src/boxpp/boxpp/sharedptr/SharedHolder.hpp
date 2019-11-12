#ifndef __BOXPP_SHAREDPTR_SHAREDHOLDER_HPP__
#define __BOXPP_SHAREDPTR_SHAREDHOLDER_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#ifndef __BOXPP_SHAREDPTR_SHAREDBASE_HPP__
#include <boxpp/sharedptr/SharedCount.hpp>
#endif

#ifndef __BOXPP_UTILS_MOVABLE_HPP__
#include <boxpp/utils/Movable.hpp>
#endif

namespace boxpp {
	namespace sharedptr {
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
			FASTINLINE operator bool() const { return Counter && Counter->Strongs > 0; }
			FASTINLINE bool operator !() const { return !Counter || Counter->Strongs <= 0; }

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

#endif // !__BOXPP_SHAREDPTR_SHAREDHOLDER_HPP__