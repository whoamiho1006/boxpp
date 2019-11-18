#ifndef __BOXPP_ASYNC_THREADLOCAL_HPP__
#define __BOXPP_ASYNC_THREADLOCAL_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/containers/SortedArray.hpp>
#include <atomic>

namespace boxpp {
	namespace async {

		template<typename Type>
		class TThreadLocal
		{
		public:
			TThreadLocal()
			{

			}

			~TThreadLocal()
			{

			}

		private:
			std::atomic_flag Atomic;
			

		};

	}
}

#endif // !__BOXPP_ASYNC_THREADLOCAL_HPP__