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
				bool bRetVal = false;

				if (void* NativeHandle = Thread->GetRaw()) {
					FBarriorScope Guard(Barrior);

					for (IThreadLocal* TLS : ThreadLocals)
						TLS->OnThreadShutdown(NativeHandle);
				}

				return bRetVal;
			}

		}
	}
}
