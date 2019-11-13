#include "Allocator.hpp"

#include <malloc.h>

namespace boxpp
{
	FAllocator::FAllocator()
	{
	}

	FAllocator::FAllocator(const TWeakPtr<IBox, ESharedMode::Safe>& Upper)
	{
		if (FAllocator* Allocator = dynamic_cast<FAllocator*>(Upper->GetAllocator())) {
			this->Upper = Allocator->ToWeak();
		}
	}

	FAllocator::FAllocator(const TWeakPtr<IAllocator, ESharedMode::Safe>& Upper)
		: Upper(Upper)
	{
	}

	FAllocator::~FAllocator()
	{
	}
	
	void* FAllocator::Alloc(size_t Size)
	{
		void* Allocated = nullptr;

		if (Upper)
			Allocated = Upper->Alloc(Size);

		// TODO: replace malloc to more suitable allocation method.
		else {
			Allocated = malloc(Size);
		}

		return Allocated;
	}

	void* FAllocator::Realloc(void* Block, size_t Size)
	{
		// TODO: replace malloc to more suitable allocation method.
		if (Upper)
			return Upper->Realloc(Block, Size);

		return realloc(Block, Size);
	}

	void FAllocator::Free(void* Block)
	{
		// TODO: replace malloc to more suitable allocation method.
		if (Upper)
			Upper->Free(Block);

		else ::free(Block);
	}
}