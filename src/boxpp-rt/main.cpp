#include <stdio.h>
#include "main.hpp"

using namespace boxpp_rt;

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
	//SBoxInterface Box;
	int RetVal = 0;

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

	/*
	if (bxLoadInterface(Box)) {
		if (Box.Enter(&Runtime)) {
			Box.Exec && !Box.Exec(&Runtime);

			if (Box.Leave(&Runtime))
				RetVal = Runtime.ExitCode; 

			else {
				fprintf(stderr, "Box couldn't be finalized correctly.\n");
				RetVal = -1;
			}
		}

		else {
			fprintf(stderr, "Box couldn't be initialized correctly.\n");
			RetVal = -1;
		}

		bxUnloadInterface(Box);
	}

	else {
		fprintf(stderr, "Box couldn't be loaded.\n");
		RetVal = -1;
	}
	*/

	return RetVal;
}