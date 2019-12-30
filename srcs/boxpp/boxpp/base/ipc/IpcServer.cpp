#include "IpcServer.hpp"
#include <boxpp/base/ipc/IpcClient.hpp>
#include <boxpp/base/tpls/pools/Mempool.h>

#if PLATFORM_WINDOWS
#include "windows/IpcServer_Windows.hpp"
#include "windows/IpcClient_Windows.hpp"
namespace boxpp {
	using FIpcServer_Impl = ipc_internals::FIpcServer_Windows;
	using FIpcClient_Impl = ipc_internals::FIpcClient_Windows;
}
#else
#include "unix/IpcServer_Unix.hpp"
#include "unix/IpcClient_Unix.hpp"
namespace boxpp {
	using FIpcServer_Impl = ipc_internals::FIpcServer_Unix;
	using FIpcClient_Impl = ipc_internals::FIpcClient_Unix;
}
#endif

namespace boxpp {
	static TMempool<FIpcServer_Impl, 2> IPCSERVER_ImplPool;

	FIpcServer::FIpcServer()
		: Impl (nullptr)
	{
	}

	template<typename CharType>
	FASTINLINE FIpcServer_Impl* IPCSERVER_CreateImpl(const CharType* Name) {
		if (FIpcServer_Impl* RImpl = IPCSERVER_ImplPool.Alloc(1)) {
			new (RImpl) FIpcServer_Impl(Name);

			if (RImpl->IsBusy() || RImpl->HasError()) {
				(*RImpl).~FIpcServer_Impl();
				IPCSERVER_ImplPool.Free(RImpl);
				RImpl = nullptr;
			}

			return RImpl;
		}

		return nullptr; 
	}
	
	bool FIpcServer::Listen(const ansi_t* Name)
	{
		if (!Impl) {
			return (Impl = IPCSERVER_CreateImpl(Name)) != nullptr;
		}

		return false;
	}

	bool FIpcServer::Listen(const wide_t* Name)
	{
		if (!Impl) {
			return (Impl = IPCSERVER_CreateImpl(Name)) != nullptr;
		}

		return false;
	}

	bool FIpcServer::Close()
	{
		if (FIpcServer_Impl* RImpl = (FIpcServer_Impl*)Impl) {
			(*RImpl).~FIpcServer_Impl();
			IPCSERVER_ImplPool.Free(RImpl);

			Impl = nullptr;
			return true;
		}

		return false;
	}

	bool FIpcServer::Accept(FIpcClient& OutContext)
	{
		if (FIpcServer_Impl* RImpl = (FIpcServer_Impl*)Impl) {
			while (true) {
				void* CImpl = RImpl->Accept();

				if (CImpl) {
					OutContext = FIpcClient(CImpl);
					break;
				}

				if (RImpl->IsBusy() || 
					RImpl->HasError()) 
				{
					Close();
					return false;
				}
			}

			return true;
		}

		return false;
	}
}