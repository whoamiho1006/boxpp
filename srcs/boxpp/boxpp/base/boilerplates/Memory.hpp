#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	/* Memory. */
	class BOXPP FMemory
	{
	public:
		static void* PureMalloc(size_t Size);
		static void* PureRealloc(void* Block, size_t Size);
		static void PureFree(void* Block);

	public:
		static void* PooledMalloc(size_t Size);
		static void* PooledRealloc(void* Block, size_t Size);
		static void PooledFree(void* Block);

	public:
#if BOX_ENFORCE_GLOBAL_POOL
		FASTINLINE static void* Malloc(size_t Size) { return PooledMalloc(Size); }
		FASTINLINE static void* Realloc(void* Block, size_t Size) { return PooledRealloc(Block, Size); }
		FASTINLINE static void Free(void* Block) { PooledFree(Block); }
#else
		FASTINLINE static void* Malloc(size_t Size) { return PureMalloc(Size); }
		FASTINLINE static void* Realloc(void* Block, size_t Size) { return PureRealloc(Block, Size); }
		FASTINLINE static void Free(void* Block) { PureFree(Block); }
#endif
	};
	
	/*
		Declare boilerplate operator overloads.
	*/
#define BOXPP_DECLARE_MEMORY_BOILERPLATE() \
	void* operator new (boxpp::size_t Size) { return boxpp::FMemory::Malloc(Size); } \
	void* operator new[] (boxpp::size_t Size) { return boxpp::FMemory::Malloc(Size); } \
	void operator delete(void* Block) { boxpp::FMemory::Free(Block); } \
	void operator delete[](void* Block) { boxpp::FMemory::Free(Block); }
}