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
	static TMempool<FIpcClient_Impl, 2> IPCCLIENT_ImplPool;

	bool FIpcClient::IsAlive() const
	{
		if (FIpcClient_Impl* RImpl = (FIpcClient_Impl*)Impl) {
			return !RImpl->IsBroken();
		}

		return false;
	}

	template<typename CharType>
	FASTINLINE FIpcClient_Impl* IPCCLIENT_CreateImpl(const CharType* Name) {
		if (FIpcClient_Impl* RImpl = IPCCLIENT_ImplPool.Alloc(1)) {
			new (RImpl) FIpcClient_Impl(Name);

			if (RImpl->IsBroken()) {
				(*RImpl).~FIpcClient_Windows();
				IPCCLIENT_ImplPool.Free(RImpl);
				RImpl = nullptr;
			}

			return RImpl;
		}

		return nullptr;
	}

	bool FIpcClient::Connect(const ansi_t* Name)
	{
		if (!Impl) {
			return (Impl = IPCCLIENT_CreateImpl(Name)) != nullptr;
		}

		return false;
	}

	bool FIpcClient::Connect(const wide_t* Name)
	{
		if (!Impl) {
			return (Impl = IPCCLIENT_CreateImpl(Name)) != nullptr;
		}

		return false;
	}

	ssize_t FIpcClient::Receive(void* Buffer, size_t Size)
	{
		if (FIpcClient_Impl* RImpl = (FIpcClient_Impl*)Impl) {
			return RImpl->Receive(Buffer, Size);
		}

		return -1;
	}

	ssize_t FIpcClient::Send(const void* Buffer, size_t Size)
	{
		if (FIpcClient_Impl* RImpl = (FIpcClient_Impl*)Impl) {
			return RImpl->Send(Buffer, Size);
		}

		return -1;
	}

	bool FIpcClient::Close()
	{
		if (FIpcClient_Impl* RImpl = (FIpcClient_Impl*)Impl) {
			if (!RImpl->IsBroken()) {
				RImpl->Close();
			}

			if (bDeployedImpl) {
				delete RImpl;
			}
			else {
				(*RImpl).~FIpcClient_Impl();
				IPCCLIENT_ImplPool.Free(RImpl);
			}

			RImpl = nullptr;
			return true;
		}

		return false;
	}
}