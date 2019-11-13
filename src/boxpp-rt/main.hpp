#pragma once
#include "loader.hpp"

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