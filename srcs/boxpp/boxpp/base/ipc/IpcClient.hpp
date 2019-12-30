#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/Movable.hpp>

namespace boxpp
{
	class FIpcServer;
	class BOXPP FIpcClient
	{
		friend class FIpcServer;

	public:
		FIpcClient() : Impl(nullptr), bDeployedImpl(false) { }
		FIpcClient(FIpcClient&& Other) 
			: Impl(nullptr), bDeployedImpl(false)
		{
			Swap(Impl, Other.Impl);
			Swap(bDeployedImpl, Other.bDeployedImpl);
		}

		~FIpcClient() { Close(); }

	protected:
		FASTINLINE FIpcClient(void* Impl) : Impl(Impl), bDeployedImpl(true) { }

	public:
		FIpcClient(const FIpcClient&) = delete;
		FIpcClient& operator =(const FIpcClient&) = delete;

	public:
		FASTINLINE FIpcClient& operator =(FIpcClient&& Other) {
			if (this != &Other) {
				Swap(Impl, Other.Impl);
				Swap(bDeployedImpl, Other.bDeployedImpl);
			}

			return *this;
		}

	private:
		void* Impl;
		bool bDeployedImpl;

	public:
		bool IsAlive() const;

		bool Connect(const ansi_t* Name);
		bool Connect(const wide_t* Name);

		ssize_t Receive(void* Buffer, size_t Size);
		ssize_t Send(const void* Buffer, size_t Size);

		bool Close();

	};

}