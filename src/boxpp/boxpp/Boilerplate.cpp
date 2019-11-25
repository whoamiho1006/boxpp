#include "Boilerplate.hpp"
#include <malloc.h>

BOXPP_DECLARE_BOILERPLATE();

namespace boxpp {
	namespace boilerplates {
		struct MetaData
		{
			size_t Size;
		};

		/*
			TODO: replace alloc method to more suitable implementation.
		*/
		void* FBoilerplate::Alloc(size_t Size)
		{
			if (MetaData* Block = (MetaData*)(malloc(sizeof(MetaData) + Size))) {
				Block->Size = Size;
				return Block + 1;
			}

			return nullptr;
		}

		size_t FBoilerplate::SizeOf(void* _Block)
		{
			if (_Block) {
				return ((MetaData*)_Block - 1)->Size;
			}

			return 0;
		}

		void* FBoilerplate::Realloc(void* _Block, size_t NewSize)
		{
			if (MetaData* Block = (MetaData*)(realloc(
				((MetaData*)_Block - 1), sizeof(MetaData) + NewSize))) 
			{
				Block->Size = NewSize;
				return Block + 1;
			}

			return nullptr;
		}

		void FBoilerplate::Free(void* _Block)
		{
			if (_Block) {
				MetaData* Block = ((MetaData*)_Block - 1);

				::free(Block);
			}
		}
	}
}
