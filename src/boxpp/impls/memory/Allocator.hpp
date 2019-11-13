#pragma once
#include <boxpp/memory/Allocator.hpp>
#include <boxpp/sharedptr/BroadcastedObject.hpp>

namespace boxpp
{
	class FAllocator : public IAllocator, 
		public IBroadcastedObject<FAllocator, ESharedMode::Safe>
	{
	public:
		FAllocator();
		FAllocator(const TWeakPtr<IBox, ESharedMode::Safe>& Upper);
		FAllocator(const TWeakPtr<IAllocator, ESharedMode::Safe>& Upper);
		~FAllocator();

	private:
		TWeakPtr<IAllocator, ESharedMode::Safe> Upper;

	public:
		virtual void* Alloc(size_t Size) override;
		virtual void* Realloc(void* Block, size_t Size) override;
		virtual void Free(void* Block) override;
	};

}