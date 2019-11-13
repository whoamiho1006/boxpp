#include "main.hpp"
#include <malloc.h>

boxpp_rt::IBoxRuntime* REF_Runtime = nullptr;

struct SAllocInfo
{
	boxpp::IAllocator* Allocator;
};

FASTINLINE void* StubAlloc(boxpp::size_t Size) {
	SAllocInfo* Allocation = nullptr;

	if (REF_Runtime) {
		if (boxpp::IBox* Box = REF_Runtime->GetBox())
		{
			boxpp::IAllocator* Alloc = Box->GetAllocator();
			Allocation = (SAllocInfo*)Alloc->Alloc(sizeof(SAllocInfo) + Size);

			Allocation->Allocator = Alloc;
		}
	}

	if (!Allocation) {
		Allocation = (SAllocInfo*)malloc(sizeof(SAllocInfo) + Size);
		Allocation->Allocator = nullptr;
	}

	return Allocation + 1;
}

void* operator new (boxpp::size_t Size) {
	return StubAlloc(Size);
}

void* operator new[](boxpp::size_t Size) {
	return StubAlloc(Size);
}

void operator delete(void* Block) {
	SAllocInfo* Allocation = ((SAllocInfo*)Block) - 1;

	if (Allocation->Allocator)
		Allocation->Allocator->Free(Allocation);

	else ::free(Allocation);
}

void operator delete[](void* Block) {
	SAllocInfo* Allocation = ((SAllocInfo*)Block) - 1;

	if (Allocation->Allocator)
		Allocation->Allocator->Free(Allocation);

	else ::free(Allocation);
}