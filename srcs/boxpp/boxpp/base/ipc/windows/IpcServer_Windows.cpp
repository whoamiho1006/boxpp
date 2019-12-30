#include "IpcServer_Windows.hpp"

#if PLATFORM_WINDOWS

#include <boxpp/base/opacities/vsprintf.hpp>
#include <boxpp/base/cryptography/internals/Djb-2.hpp>
#include <boxpp/base/tpls/strings/String.hpp>

#define IPC_IMPL_INCLUDES	1
#include "IpcMakePipeName.hpp"
#undef IPC_IMPL_INCLUDES

#include "IpcClient_Windows.hpp"

#include <Windows.h>

namespace boxpp {
	namespace ipc_internals {
		class FIpcContext_Windows : public FIpcClient_Windows
		{
			friend class FIpcServer_Windows;

		protected:
			FIpcContext_Windows(void* hPipe)
				: FIpcClient_Windows(hPipe)
			{
			}

		public:
			virtual bool Close() override {
				if (hPipe != INVALID_HANDLE_VALUE) {
					DisconnectNamedPipe(hPipe);
					CloseHandle(hPipe);
					hPipe = INVALID_HANDLE_VALUE;
					return true;
				}

				return false;
			}
		};

		FIpcServer_Windows::FIpcServer_Windows(const ansi_t* Name, size_t PipeWidth)
			: hCurrentPipe(INVALID_HANDLE_VALUE), PipeWidth(PipeWidth), bAnsiName(true),
			  bPipeBusy(false), bPipeError(false)
		{
			ZeroMemory(PipeNameA, sizeof(PipeNameA));
			IPC_MakePipeName(PipeNameA, Name);
			RearmPipe();
		}

		FIpcServer_Windows::FIpcServer_Windows(const wide_t* Name, size_t PipeWidth)
			: hCurrentPipe(INVALID_HANDLE_VALUE), PipeWidth(PipeWidth), bAnsiName(false),
			  bPipeBusy(false), bPipeError(false)
		{
			ZeroMemory(PipeNameW, sizeof(PipeNameW));
			IPC_MakePipeName(PipeNameW, Name);
			RearmPipe();
		}

		FIpcServer_Windows::~FIpcServer_Windows() {
			if (hCurrentPipe != INVALID_HANDLE_VALUE) {
				CloseHandle(hCurrentPipe);
			}

			hCurrentPipe = INVALID_HANDLE_VALUE;
		}

		FIpcClient_Windows* FIpcServer_Windows::Accept()
		{
			FIpcContext_Windows* RetVal = nullptr;

			while (!bPipeBusy && !bPipeError) {
				bool bConnected = ConnectNamedPipe(hCurrentPipe, NULL) ||
					(GetLastError() == ERROR_PIPE_CONNECTED);

				if (!bConnected) {
					CloseHandle(hCurrentPipe);
					RearmPipe();
					continue;
				}

				RetVal = new FIpcContext_Windows(hCurrentPipe);
				RearmPipe();
				break;
			}

			return RetVal;
		}

		bool FIpcServer_Windows::RearmPipe()
		{
			if (!bPipeBusy && !bPipeError) {
				if (bAnsiName) {
					hCurrentPipe = CreateNamedPipeA(PipeNameA, PIPE_ACCESS_DUPLEX,
						PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
						PIPE_UNLIMITED_INSTANCES, DWORD(PipeWidth), DWORD(PipeWidth), 0, NULL);
				}
				else {
					hCurrentPipe = CreateNamedPipeW(PipeNameW, PIPE_ACCESS_DUPLEX,
						PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
						PIPE_UNLIMITED_INSTANCES, DWORD(PipeWidth), DWORD(PipeWidth), 0, NULL);
				}

				if (hCurrentPipe == INVALID_HANDLE_VALUE) {
					bPipeBusy = (GetLastError() == ERROR_PIPE_BUSY);
					bPipeError = (GetLastError() != ERROR_PIPE_BUSY);
				}
			}

			return !bPipeBusy && !bPipeError;
		}
	}
}
#endif