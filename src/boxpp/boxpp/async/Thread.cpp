#include "Thread.hpp"
#include "ThreadLocal.hpp"

namespace boxpp {
	namespace async {
		void FThread::OnThreadShutdown()
		{
			_::FThreadLocalServer::Get().Shutdown(this);
		}
	}
}
