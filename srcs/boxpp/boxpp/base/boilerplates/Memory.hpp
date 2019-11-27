#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	/* Memory. */
	class BOXPP FMemory
	{
	public:
		static void* Malloc(size_t Size);
		static void* Realloc(void* Block, size_t Size);
		static void Free(void* Block);
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