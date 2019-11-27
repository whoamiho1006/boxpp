#include "Memory.hpp"

#include <malloc.h>

BOXPP_DECLARE_MEMORY_BOILERPLATE();

namespace boxpp {

	void* FMemory::Malloc(size_t Size)
	{
		return ::malloc(Size);
	}

	void* FMemory::Realloc(void* Block, size_t Size)
	{
		return ::realloc(Block, Size);
	}

	void FMemory::Free(void* Block)
	{
		::free(Block);
	}
}