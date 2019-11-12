#ifndef __BOXPP_ASYNC_IRUNNABLE_HPP__
#define __BOXPP_ASYNC_IRUNNABLE_HPP__

namespace boxpp
{
	namespace async
	{
		class IRunnable
		{
		public:
			virtual ~IRunnable() { }

		public:
			virtual void Run() = 0;
		};
	}
}

#endif