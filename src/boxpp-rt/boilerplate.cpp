#include "boilerplate.hpp"
#include <malloc.h>

#pragma comment(lib, "boxpp.lib")

BOXPP_DECLARE_BOILERPLATE();

FBoxBoilerplate* FBoxBoilerplate::Get()
{
	static FBoxBoilerplate __PLATE__;
	return &__PLATE__;
}

void* FBoxBoilerplate::Alloc(size_t Size)
{
	return malloc(Size);
}

void* FBoxBoilerplate::Realloc(void * Block, size_t NewSize)
{
	return realloc(Block, NewSize);
}

void FBoxBoilerplate::Free(void * Block)
{
	::free(Block);
}
