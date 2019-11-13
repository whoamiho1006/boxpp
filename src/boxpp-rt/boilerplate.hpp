#pragma once
#include <boxpp.hpp>
#include <boxpp/internal/IBoxRuntime.hpp>
#include <boxpp/Boilerplate.hpp>

/* Entry point. */
BOXEXTERN boxpp::s32 run(boxpp::IBox* Box);

/*
	Runtime class base.
*/
class FBoxRuntimeCommon : public boxpp_rt::IBoxRuntime
{
public:
	FBoxRuntimeCommon()
		: ExitCode(0)
	{
	}

	FBoxRuntimeCommon(const boxpp::c8** Args, boxpp::s32 Argc)
		: IBoxRuntime(Args, Argc), ExitCode(0)
	{
	}

	virtual ~FBoxRuntimeCommon() {

	}

public:
	boxpp::s32 ExitCode;

public:
	/* Run this runtime if required. */
	virtual void Run() override { 
		ExitCode = run(GetBox()); 
	};
};

class FBoxBoilerplate : public boxpp::boilerplates::IBoilerplate
{
public:
	static FBoxBoilerplate* Get();

public:
	virtual void* Alloc(size_t Size);
	virtual void* Realloc(void* Block, size_t NewSize);
	virtual void Free(void* Block);

};