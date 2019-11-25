#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp {
	namespace boilerplates {
		/*	Boilerplate that manages the allocator 
			which used for allocating memory in the entire library. */
		class BOXPP FBoilerplate {
		public:
			static void* Alloc(size_t Size);
			static size_t SizeOf(void* Block);
			static void* Realloc(void* Block, size_t NewSize);
			static void Free(void* Block);
		};

#define BOXPP_DECLARE_BOILERPLATE() \
	void* operator new (boxpp::size_t Size) { return boxpp::boilerplates::FBoilerplate::Alloc(Size); } \
	void* operator new[] (boxpp::size_t Size) { return boxpp::boilerplates::FBoilerplate::Alloc(Size); } \
	void operator delete(void* Block) { boxpp::boilerplates::FBoilerplate::Free(Block); } \
	void operator delete[](void* Block) { boxpp::boilerplates::FBoilerplate::Free(Block); }

	}
}
