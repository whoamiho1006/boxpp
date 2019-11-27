#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	/* Boilerplate implementation. */
	class BOXPP FBoilerplate
	{
	public:
		static void* Malloc(size_t Size);
		static void* Realloc(void* Block, size_t Size);
		static void Free(void* Block);
	};
	
	/*
		Declare boilerplate operator overloads.
	*/
#define BOXPP_DECLARE_BOILERPLATE() \
	void* operator new (boxpp::size_t Size) { return boxpp::FBoilerplate::Malloc(Size); } \
	void* operator new[] (boxpp::size_t Size) { return boxpp::FBoilerplate::Malloc(Size); } \
	void operator delete(void* Block) { boxpp::FBoilerplate::Free(Block); } \
	void operator delete[](void* Block) { boxpp::FBoilerplate::Free(Block); }
}