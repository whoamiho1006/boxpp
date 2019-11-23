#pragma once
#include <boxpp.hpp>
#include <boxpp/Boilerplate.hpp>

/* Entry point. */
//BOXEXTERN boxpp::s32 run(boxpp::IBox* Box);
NO_MANGLED BOXEXTERN void* BOX_ExeRun();

class FBoxBoilerplate : public boxpp::boilerplates::IBoilerplate
{
public:
	static FBoxBoilerplate* Get();

public:
	virtual void* Alloc(size_t Size);
	virtual void* Realloc(void* Block, size_t NewSize);
	virtual void Free(void* Block);
};