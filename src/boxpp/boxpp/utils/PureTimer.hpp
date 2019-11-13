#ifndef __BOXPP_ASYNC_PURETIMER_HPP__
#define __BOXPP_ASYNC_PURETIMER_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <time.h>

namespace boxpp
{
	/* Timer that calculates elapsed time-span. */
	class FPureTimer
	{
	public:
		FASTINLINE FPureTimer()
			: Latest(clock())
		{ 
		}

		FASTINLINE ~FPureTimer()
		{ 
		}

	private:
		clock_t Latest;

	public:
		/* Reset timer. */
		FASTINLINE void Reset() {
			Latest = clock();
		}

		/* Get elapsed milliseconds. */
		FASTINLINE s32 GetMilliseconds() const {
			return (s32)(((clock() - Latest) * 1.0f / CLOCKS_PER_SEC) * 1000);
		}

		/* Get elapsed seconds. */
		FASTINLINE float GetSeconds() const {
			return (clock() - Latest) * 1.0f / CLOCKS_PER_SEC;
		}
	};
}

#endif // !__BOXPP_ASYNC_PURETIMER_HPP__