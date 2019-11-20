#ifndef __BOXPP_ASYNC_THREADLOCAL_HPP__
#define __BOXPP_ASYNC_THREADLOCAL_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Barrior.hpp>
#include <boxpp/containers/Map.hpp>
#include <boxpp/utils/Function.hpp>
#include <atomic>

#if BOX_COMPILE_BODY
namespace boxpp_rt {
	class FBoxRuntime;
}
#endif

namespace boxpp {
	namespace async {
		class FThread;

		template<typename Type>
		class TThreadLocal;

		namespace _ {
			class FThreadLocalServer;

			/*
				TLS Interface.
			*/
			class IThreadLocal
			{
				friend class FThreadLocalServer;

			public:
				virtual ~IThreadLocal() { }

			protected:
				virtual void OnThreadShutdown(void* Thread) = 0;
			};

			/*
				TLS Server.
			*/
			class BOXPP FThreadLocalServer
			{
				template<typename> 
				friend class TThreadLocal;
				friend class FThread;

#if BOX_COMPILE_BODY
				friend class boxpp_rt::FBoxRuntime;
#endif

			private:
				FThreadLocalServer() { }
				~FThreadLocalServer() { }

			public:
				static FThreadLocalServer& Get();

			private:
				FBarrior Barrior;
				std::atomic_flag Atomic;
				TSortedArray<IThreadLocal*> ThreadLocals;

			protected:
				void Register(IThreadLocal* TLS);
				void Unregister(IThreadLocal* TLS);

			protected:
				bool Shutdown(FThread* Thread);
				bool ShutdownNative(void* NativeThread);
			};
		}

		/*
			Cross platform TLS implementation
			using TMap by Binary Search.

			The finalizer for configured values can be called 
			for only FThread wrapped threads.
			
			otherwise, they will be called at destruction time.
		*/
		template<typename Type>
		class TThreadLocal : public _::IThreadLocal
		{
		public:
			TThreadLocal()
			{
				_::FThreadLocalServer::Get()
					.Register(this);
			}

			~TThreadLocal()
			{
				TArray<void*> Threads;

				Lock();
				Values.GetKeys(Threads);
				Unlock();

				for (void* Thread : Threads)
				{
					OnThreadShutdown(Thread);
				}

				_::FThreadLocalServer::Get()
					.Unregister(this);
			}

		public:
			FASTINLINE void SetPrepare(TFunction<Type*()> Functor) {
				Prepare = Forward<TFunction<Type*()>>(Functor);
			}

			FASTINLINE void SetCleanup(TFunction<void(Type*)> Functor) {
				Cleanup = Forward<TFunction<void(Type*)>>(Functor);
			}

		private:
			FASTINLINE void Lock() const {
				while (!(Atomic).test_and_set(std::memory_order_acquire));
			}

			FASTINLINE void Unlock() const {
				(Atomic).clear(std::memory_order_release);
			}

		public:
			/* Set TLS Value. */
			FASTINLINE Type* Set(Type* Value)
			{
				void* NativeHandle = FThread::SelfNative();
				Type* Previous = nullptr;

				Lock();
				if (Values.ContainsKey(NativeHandle)) {
					Previous = Values[NativeHandle];
				}

				Values.Emplace(NativeHandle, Value);
				Unlock();

				if (Previous && Cleanup) {
					Cleanup(Previous);
				}

				return Value;
			}

			/* Get TLS Value. */
			FASTINLINE Type* Get() const
			{
				void* NativeHandle = FThread::SelfNative();
				Type* Value = nullptr;

				Lock();
				if (Values.ContainsKey(NativeHandle)) {
					Value = Values[NativeHandle];
				}

				if (!Value && Prepare) {
					Values.Emplace(NativeHandle, Value = Prepare());
				}

				Unlock();


				return Value;
			}

		protected:
			virtual void OnThreadShutdown(void* Thread) override {
				Type* Value = nullptr;

				Lock();
				if (Values.ContainsKey(Thread)) {
					Value = Values[Thread];
					Values.Remove(Thread);
				}
				Unlock();

				if (Value && Cleanup) {
					Cleanup(Value);
				}
			}

		private:
			mutable std::atomic_flag Atomic;
			mutable TMap<void*, Type*> Values;
			TFunction<Type*()> Prepare;
			TFunction<void(Type*)> Cleanup;

		};

	}
}

#endif // !__BOXPP_ASYNC_THREADLOCAL_HPP__