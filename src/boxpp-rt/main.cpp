#include "boilerplate.hpp"
#include <stdio.h>

using namespace boxpp::boilerplates;

int main(int argc, char** argv)
{
	FBoilerplate::Set(FBoxBoilerplate::Get());
	int RetVal = 0;

	/*
	if (bxEnterRuntime(&Runtime))
	{
		bxExecRuntime(&Runtime);

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
	*/

	FBoilerplate::Set(nullptr);
	return RetVal;
}
