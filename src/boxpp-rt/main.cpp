#include "boilerplate.hpp"
#include <stdio.h>

using namespace boxpp_rt;
using namespace boxpp::boilerplates;

/* box runtime class for CLI */
class FBoxRuntimeExe : public FBoxRuntimeCommon
{
public:
	FBoxRuntimeExe(int argc, const boxpp::c8** argv) 
		: FBoxRuntimeCommon(argv, argc)
	{ }

public:
	/* Get type of this runtime. */
	virtual ERuntimeType GetType() const override { return ERuntimeType::Exe; }
};

int main(int argc, char** argv)
{
	FBoxRuntimeExe Runtime(argc, (const boxpp::c8**) argv);
	int RetVal = 0;

	if (bxEnterRuntime(&Runtime))
	{
		FBoilerplate::Set(FBoxBoilerplate::Get());
		bxExecRuntime(&Runtime);

		FBoilerplate::Set(nullptr);
		if (bxLeaveRuntime(&Runtime))
			RetVal = Runtime.ExitCode;

		else {
			fprintf(stderr, "Box couldn't be finalized correctly.\n");
			RetVal = -1;
		}
	}
	else {
		fprintf(stderr, "Box couldn't be loaded.\n");
		RetVal = -1;
	}


	return RetVal;
}
