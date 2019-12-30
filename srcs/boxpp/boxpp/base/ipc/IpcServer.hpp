#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/Movable.hpp>

namespace boxpp
{
	class FIpcClient;

	/*
		IPC Server.
		1. Windows: using Named-pipe.
		2. Unix systems: using Unix Domain Socket.

		Name to path conversion:
			- Windows: \\\\.\\pipe\\boxpp-[HASH VALUE]
			- Unix: /tmp/boxpp/[HASH VALUE].ipc

		Hashing algorithm: DJB-2.
	*/
	class BOXPP FIpcServer
	{
	public:
		FIpcServer();
		FIpcServer(FIpcServer&& Other) : Impl(nullptr) { Swap(Impl, Other.Impl); }
		~FIpcServer() { Close(); }

	public:
		FIpcServer(const FIpcServer&) = delete;
		FIpcServer& operator =(const FIpcServer&) = delete;

	public:
		FASTINLINE FIpcServer& operator =(FIpcServer&& Other) {
			if (this != &Other) {
				Swap(Impl, Other.Impl);
			}

			return *this;
		}

	private:
		void* Impl;

	public:
		FASTINLINE bool IsListening() const { return Impl; }

	public:
		bool Listen(const ansi_t* Name);
		bool Listen(const wide_t* Name);
		bool Close();

	public:
		bool Accept(FIpcClient& OutContext);
	};

}