#include "Memory.hpp"

#include <boxpp/base/systems/Debugger.hpp>
#include <boxpp/base/tpls/pools/Fastpool.hpp>

#include <malloc.h>
#include <errno.h>

BOXPP_DECLARE_MEMORY_BOILERPLATE();

namespace boxpp {

	FFastpool& GetGlobalPool() {
		static FFastpool _Pool(BOX_GLOBAL_POOL_ALIGNMENT);
		return _Pool;
	}

	void* FMemory::PureMalloc(size_t Size)
	{
		return ::malloc(Size);
	}

	void* FMemory::PureRealloc(void* Block, size_t Size)
	{
		return ::realloc(Block, Size);
	}

	void FMemory::PureFree(void* Block)
	{
		::free(Block);
	}

	void* FMemory::PooledMalloc(size_t Size)
	{
		return GetGlobalPool().Alloc(Size);
	}

	void* FMemory::PooledRealloc(void* Block, size_t Size)
	{
		return GetGlobalPool().Realloc(Block, Size);
	}

	void FMemory::PooledFree(void* Block)
	{
		GetGlobalPool().Free(Block);
	}
}
