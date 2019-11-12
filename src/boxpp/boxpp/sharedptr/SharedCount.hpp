#ifndef __BOXPP_SHAREDPTR_SHAREDBASE_HPP__
#define __BOXPP_SHAREDPTR_SHAREDBASE_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <atomic>

namespace boxpp {
	enum class ESharedMode
	{
		/* Unsafe, but more faster than safe. */
		Unsafe = 0,

		/* Thread-safe, but slower than unsafe. */
		Safe
	};

	namespace sharedptr {

		template<ESharedMode Mode>
		struct FSharedOps;

		class ISharedCount
		{
			template<ESharedMode> 
			friend struct FSharedOps;

		public:
			ISharedCount() : Weaks(0), Strongs(0) { }
			virtual ~ISharedCount() { }

		protected:
			std::atomic_flag Atomic;
			s32 Weaks;
			s32 Strongs;

		protected:
			virtual void Delete() = 0;
		};

#		define SHAREDPTR_DO_WITH_ATOMIC(Atomic, ...) { \
			while ((Atomic).test_and_set(std::memory_order_acquire)); __VA_ARGS__; \
			(Atomic).clear(std::memory_order_release); }

		template<ESharedMode Mode> struct FSharedOps
		{
			/* -- Add references. -- */
			FASTINLINE static void AddWeaks(ISharedCount* Shared) { ++Shared->Weaks; }
			FASTINLINE static void AddStrongs(ISharedCount* Shared) { ++Shared->Strongs; }

			/* -- Subtract references. -- */
			FASTINLINE static bool SubWeaks(ISharedCount* Shared) { return !(--Shared->Weaks); }
			FASTINLINE static bool SubStrongs(ISharedCount* Shared) { return !(--Shared->Strongs); }

			/* -- Delete object. -- */
			FASTINLINE static void DeleteObject(ISharedCount* Shared) { Shared->Delete(); }
			FASTINLINE static void Delete(ISharedCount* Shared) { delete Shared; }
		};

		template<> struct FSharedOps<ESharedMode::Safe>
		{
			/* -- Add references. -- */
			FASTINLINE static void AddWeaks(ISharedCount* Shared) { SHAREDPTR_DO_WITH_ATOMIC(Shared->Atomic, ++Shared->Weaks); }
			FASTINLINE static void AddStrongs(ISharedCount* Shared) { SHAREDPTR_DO_WITH_ATOMIC(Shared->Atomic, ++Shared->Strongs); }

			/* -- Subtract references. -- */
			FASTINLINE static bool SubWeaks(ISharedCount* Shared) { bool bRetVal = false; SHAREDPTR_DO_WITH_ATOMIC(Shared->Atomic, bRetVal = !(--Shared->Weaks)); return bRetVal; }
			FASTINLINE static bool SubStrongs(ISharedCount* Shared) { bool bRetVal = false; SHAREDPTR_DO_WITH_ATOMIC(Shared->Atomic, bRetVal = !(--Shared->Strongs)); return bRetVal; }

			/* -- Delete object. -- */
			FASTINLINE static void DeleteObject(ISharedCount* Shared) { Shared->Delete(); }
			FASTINLINE static void Delete(ISharedCount* Shared) { delete Shared; }
		};

		template<typename ObjectType>
		struct TDefaultDeleter
		{
			FASTINLINE void operator ()(ObjectType* Object) {
				delete Object;
			}
		};

		template<typename ObjectType, typename DeleterType = TDefaultDeleter<ObjectType>>
		class TSharedCount : DeleterType, public ISharedCount
		{
		public:
			TSharedCount(ObjectType* Object)
				: Object(Object)
			{
			}

			TSharedCount(ObjectType* Object, DeleterType&& Deleter) 
				: DeleterType(Deleter), Object(Object)
			{
			}

		private:
			ObjectType* Object;

		protected:
			virtual void Delete() override { (*static_cast<DeleterType*>(this))(Object); }
		};
	}
}

#endif