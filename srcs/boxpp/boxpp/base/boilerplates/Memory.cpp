#include "Memory.hpp"
#include <boxpp/base/systems/Debugger.hpp>

#include <malloc.h>
#include <errno.h>

BOXPP_DECLARE_MEMORY_BOILERPLATE();

namespace boxpp {

	void* FMemory::Malloc(size_t Size)
	{
		void* p = ::malloc(Size);

		BOX_ASSERT(errno == ENOMEM, "No Memory!");

		return p;
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

#include <Windows.h>
void test() {

}