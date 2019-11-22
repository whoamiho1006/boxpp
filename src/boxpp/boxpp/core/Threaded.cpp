#include "Threaded.hpp"

#include <boxpp/hints/windows.hpp>
#include <boxpp/hints/pthread.hpp>

namespace boxpp {

	void IThreaded::Deploy::Configure(IThreaded* Threaded) {
#if PLATFORM_WINDOWS
		Threaded->RawHandle = w32_compat::GetCurrentThread();
#endif
#if PLATFORM_POSIX
		Threaded->RawHandle = pthread_self();
#endif
	}
}