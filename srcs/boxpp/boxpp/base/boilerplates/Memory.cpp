#include "Boilerplate.hpp"

#include <malloc.h>

BOXPP_DECLARE_BOILERPLATE();

namespace boxpp {

	void* FBoilerplate::Malloc(size_t Size)
	{
		return ::malloc(Size);
	}

	void* FBoilerplate::Realloc(void* Block, size_t Size)
	{
		return ::realloc(Block, Size);
	}

	void FBoilerplate::Free(void* Block)
	{
		::free(Block);
	}
}