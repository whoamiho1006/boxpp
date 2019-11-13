#ifndef __BOXPP_ASYNC_ASYNC_HPP__
#define __BOXPP_ASYNC_ASYNC_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/async/Task.hpp>
#include <boxpp/async/TaskSource.hpp>
#include <boxpp/async/IRunnable.hpp>

namespace boxpp {
	namespace async {



	}

	template<typename ReturnType>
	FASTINLINE TTask<ReturnType> Async(const TSharedPtr<async::IRunnable>& Runnable)
	{

		return nullptr;
	}
}

#endif // !__BOXPP_ASYNC_ASYNC_HPP__