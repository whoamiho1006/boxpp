#include <boxpp.hpp>
#include <boxpp/internal/IBoxRuntime.hpp>
#include <boxpp/Boilerplate.hpp>

using namespace boxpp_rt;
using namespace boxpp::boilerplates;

BOXPP_DECLARE_BOILERPLATE();

/* Entry point. */
BOXEXTERN bool moduleStartup(boxpp::IBox* Box);
BOXEXTERN void moduleShutdown(boxpp::IBox* Box);

class FBoxRuntimeDll : public IBoxRuntime
{
public:
	FBoxRuntimeDll()
	{
	}

	virtual ~FBoxRuntimeDll() {

	}

public:
	/* Get type of this runtime. */
	virtual ERuntimeType GetType() const override { return ERuntimeType::Module; }

	/* Run this runtime if required. */
	virtual void Run() override {
		if (boxpp::IBox* Box = GetBox()) {
			if (moduleStartup(Box)) {
				moduleShutdown(Box);
			}
		}
	};
};

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
	FBoxRuntimeDll Runtime;
	
	if (bxEnterRuntime(&Runtime)) {
		FBoilerplate::Set(
			FBoxBoilerplateDll::Get()
		);

		bxExecRuntime(&Runtime);

		FBoilerplate::Set(nullptr);
		bxLeaveRuntime(&Runtime);
	}
}
