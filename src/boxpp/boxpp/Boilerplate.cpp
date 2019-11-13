#include "Boilerplate.hpp"
#include <malloc.h>

namespace boxpp {
	namespace boilerplates {
		IBoilerplate* FBoilerplate::Plate = nullptr;

		struct MetaData
		{
			IBoilerplate* Allocator;
		};

		IBoilerplate* FBoilerplate::Get() { return Plate; }

		/* Internal use only! */

		void FBoilerplate::Set(IBoilerplate * Boilerplate) { Plate = Boilerplate; }

		/*
			TODO: replace alloc method to more suitable implementation.
		*/
		void* FBoilerplate::Alloc(size_t Size)
		{
			MetaData* Block = nullptr;
			
			(Block = (MetaData*)(Plate ? 
				Plate->Alloc(sizeof(MetaData) + Size) : 
				malloc(sizeof(MetaData) + Size)))
				->Allocator = Plate;

			return Block + 1;
		}

		void* FBoilerplate::Realloc(void* _Block, size_t NewSize)
		{
			MetaData* Block = ((MetaData*)_Block - 1);
			IBoilerplate* Plate = Block->Allocator;

			Block = (MetaData*)(Plate ?
				Plate->Realloc(Block, sizeof(MetaData) + NewSize) :
				realloc(Block, sizeof(MetaData) + NewSize));

			return Block + 1;
		}

		void FBoilerplate::Free(void* _Block)
		{
			MetaData* Block = ((MetaData*)_Block - 1);
			IBoilerplate* Plate = Block->Allocator;

			if (Plate)
				Plate->Free(Block);

			else ::free(Block);
		}
	}
}

void* operator new (boxpp::size_t Size) {
	return boxpp::boilerplates::FBoilerplate::Alloc(Size);
}

void* operator new[] (boxpp::size_t Size) {
	return boxpp::boilerplates::FBoilerplate::Alloc(Size);
}

void operator delete(void* Block) {
	boxpp::boilerplates::FBoilerplate::Free(Block);
}

void operator delete[] (void* Block) {
	boxpp::boilerplates::FBoilerplate::Free(Block);
}