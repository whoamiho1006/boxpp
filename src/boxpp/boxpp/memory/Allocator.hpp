#ifndef __BOXPP_MEMORY_ALLOCATOR_HPP__
#define __BOXPP_MEMORY_ALLOCATOR_HPP__

#ifndef __BOXPP_HPP__
/* Loads boxpp.hpp header if not loaded. */
#include <boxpp.hpp>
#endif

namespace boxpp
{
	/* Memory manager interface. */
	class IAllocator
	{
	public:
		virtual ~IAllocator() { }

	public:
		virtual void* Alloc(size_t Size) = 0;
		virtual void* Realloc(void* Block, size_t Size) = 0;
		virtual void Free(void* Block) = 0;
	};
}

#endif // !__BOXPP_MEMORY_ALLOCATOR_HPP__
