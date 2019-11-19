#include "ThreadLocal.hpp"
#include <boxpp/async/Thread.hpp>

namespace boxpp {
	namespace async {
		namespace _ {
			FThreadLocalServer& FThreadLocalServer::Get() {
				static FThreadLocalServer Server;
				return Server;
			}

			void FThreadLocalServer::Register(IThreadLocal* TLS)
			{
				FBarriorScope Guard(Barrior);
				ThreadLocals.Add(TLS);
			}

			void FThreadLocalServer::Unregister(IThreadLocal* TLS)
			{
				FBarriorScope Guard(Barrior);
				ThreadLocals.Remove(TLS);
			}

			bool FThreadLocalServer::Shutdown(FThread* Thread) {
				if (void* NativeHandle = Thread->GetRaw()) {
					return ShutdownNative(NativeHandle);
				}

				return false;
			}

			bool FThreadLocalServer::ShutdownNative(void* NativeThread)
			{
				FBarriorScope Guard(Barrior);

				for (IThreadLocal* TLS : ThreadLocals)
					TLS->OnThreadShutdown(NativeThread);

				return true;
			}

		}
	}
}
