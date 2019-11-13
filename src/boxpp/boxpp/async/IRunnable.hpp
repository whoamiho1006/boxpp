#ifndef __BOXPP_ASYNC_IRUNNABLE_HPP__
#define __BOXPP_ASYNC_IRUNNABLE_HPP__

#include <boxpp/utils/Instrusive.hpp>

#ifndef __BOXPP_UTILS_MOVABLE_HPP__
#include <boxpp/utils/Movable.hpp>
#endif

#ifndef __BOXPP_SHAREDPTR_BORADCASTED_HPP__
#include <boxpp/sharedptr/BroadcastedObject.hpp>
#endif

namespace boxpp
{
	namespace async
	{
		/* Runnable interface. */
		class IRunnable
		{
		public:
			virtual ~IRunnable() { }

		public:
			virtual void Run() = 0;
		};

		namespace _ {
			/* Lambda runnable. */
			template<typename LambdaType>
			class TLambdaRunnable : LambdaType, public IRunnable
			{
			public:
				TLambdaRunnable(LambdaType&& Lambda)
					: LambdaType(Lambda)
				{
				}

			public:
				virtual void Run() override {
					(*static_cast<LambdaType*>(this))();
				}
			};
		}

		/* Makes lambda runnable. */
		template<typename LambdaType>
		FASTINLINE sharedptr::TSmartProxy<IRunnable> MakeRunnable(LambdaType&& Lambda) {
			return MakeShared(new _::TLambdaRunnable<LambdaType>(Lambda));
		}
	}
}

#endif