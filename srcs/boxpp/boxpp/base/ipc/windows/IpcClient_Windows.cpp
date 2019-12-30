#include "IpcClient_Windows.hpp"

#if PLATFORM_WINDOWS
#include <boxpp/base/opacities/vsprintf.hpp>
#include <boxpp/base/cryptography/internals/Djb-2.hpp>
#include <boxpp/base/tpls/strings/String.hpp>

#define IPC_IMPL_INCLUDES	1
#include "IpcMakePipeName.hpp"
#undef IPC_IMPL_INCLUDES

#include <Windows.h>

namespace boxpp 
{
	namespace ipc_internals
	{
		FIpcClient_Windows::FIpcClient_Windows(void* hPipe)
			: hPipe(hPipe),  bAnsiName(false), bUnnamed(true)
		{
		}

		FIpcClient_Windows::FIpcClient_Windows(const ansi_t* Name)
			: hPipe(INVALID_HANDLE_VALUE), bAnsiName(true), bUnnamed(false)
		{
			ZeroMemory(PipeNameA, sizeof(PipeNameA));
			IPC_MakePipeName(PipeNameA, Name);
			TryConnect();
		}

		FIpcClient_Windows::FIpcClient_Windows(const wide_t* Name)
			: hPipe(INVALID_HANDLE_VALUE), bAnsiName(false), bUnnamed(false)
		{
			ZeroMemory(PipeNameW, sizeof(PipeNameW));
			IPC_MakePipeName(PipeNameW, Name);
			TryConnect();
		}

		bool FIpcClient_Windows::IsBroken() const
		{ 
			return hPipe == INVALID_HANDLE_VALUE;
		}

		bool FIpcClient_Windows::TryConnect()
		{
			bool bTimedout = false;
			bool bPipeError = false;

			while (hPipe == INVALID_HANDLE_VALUE && !bTimedout) {
				bPipeError = false;

				if (bAnsiName) {
					hPipe = CreateFileA(PipeNameA,
						GENERIC_READ | GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, NULL);
				}
				else {
					hPipe = CreateFileW(PipeNameW,
						GENERIC_READ | GENERIC_WRITE,
						0, NULL, OPEN_EXISTING, 0, NULL);
				}

				if (hPipe != INVALID_HANDLE_VALUE) {
					DWORD PipeMode = PIPE_READMODE_MESSAGE;
					bPipeError = !SetNamedPipeHandleState(
						hPipe, &PipeMode, NULL, NULL);

					if (bPipeError) {
						Close();
						break;
					}

					return true;
				}

				if (GetLastError() != ERROR_PIPE_BUSY) {
					bPipeError = true;
					break;
				}

				else if (bAnsiName) {
					bTimedout = !WaitNamedPipeA(PipeNameA, 10000);
				}

				else {
					bTimedout = !WaitNamedPipeW(PipeNameW, 10000);
				}
			}

			return false;
		}

		ssize_t FIpcClient_Windows::Receive(void* Buffer, size_t Size)
		{
			if (hPipe != INVALID_HANDLE_VALUE) {
				DWORD ReceivedBytes = 0;

				if (!ReadFile(hPipe, Buffer, DWORD(Size), &ReceivedBytes, NULL) ||
					ReceivedBytes == 0)
				{
					if (GetLastError() == ERROR_BROKEN_PIPE) {
						Close();
						return -1;
					}
				}

				return ssize_t(ReceivedBytes);
			}

			return -1;
		}

		ssize_t FIpcClient_Windows::Send(const void* Buffer, size_t Size)
		{
			ssize_t RetVal = -1;

			while (hPipe != INVALID_HANDLE_VALUE) {
				DWORD SentBytes = 0;

				if (!WriteFile(hPipe, Buffer, DWORD(Size), &SentBytes, NULL) ||
					(SentBytes == 0 && Size))
				{
					Close();

					if (!SentBytes && RetVal > 0) {
						break;
					}

					RetVal = -1;
					break;
				}

				else if (Size >= SentBytes) {
					if (RetVal < 0) {
						RetVal = 0;
					}

					Buffer = (u8*)Buffer + SentBytes;
					Size -= SentBytes;
					RetVal += SentBytes;

					if (!SentBytes) {
						break;
					}
				}

				else if (Size < SentBytes) {
					if (RetVal < 0) {
						RetVal = 0;
					}

					RetVal += SentBytes;
					Size = 0;
				}

				//WriteFile
				FlushFileBuffers(hPipe);
				if (!Size) {
					if (RetVal <= 0)
						return 0;

					break;
				}
			}

			return RetVal;
		}

		bool FIpcClient_Windows::Close()
		{
			if (hPipe != INVALID_HANDLE_VALUE) {
				CloseHandle(hPipe);
				hPipe = INVALID_HANDLE_VALUE;
				return true;
			}

			return false;
		}
	}
}
#endif