#include <boxpp.hpp>
#include <boxpp/Boilerplate.hpp>

using namespace boxpp::boilerplates;

BOXPP_DECLARE_BOILERPLATE();

class FBoxBoilerplateDll : public boxpp::boilerplates::IBoilerplate
{
public:
	static FBoxBoilerplateDll* Get() {
		static FBoxBoilerplateDll __PLATE__;
		return &__PLATE__;
	}

public:
	virtual void* Alloc(size_t Size) {
		return malloc(Size);
	}
	
	virtual void* Realloc(void* Block, size_t NewSize) {
		return realloc(Block, NewSize);
	}

	virtual void Free(void* Block) {
		::free(Block);
	}
};

/* module main. */
NO_MANGLED BOXEXPORT void moduleMain() {
	FBoilerplate::Set(
		FBoxBoilerplateDll::Get()
	);

	FBoilerplate::Set(nullptr);
}
