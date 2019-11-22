#include "boilerplate.hpp"

#if PLATFORM_WINDOWS
#include <Windows.h>

using namespace boxpp::boilerplates;

int APIENTRY WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	int RetVal = 0;
	FBoilerplate::Set(FBoxBoilerplate::Get());

	/*
	FBoxRuntimeExeW32 Runtime(hInstance);

	if (bxEnterRuntime(&Runtime))
	{
		bxExecRuntime(&Runtime);

		if (bxLeaveRuntime(&Runtime))
			RetVal = Runtime.ExitCode;

		else {
			MessageBoxA(NULL, "Box couldn't be finalized correctly.", "Box", 0);
			RetVal = -1;
		}
	}
	else {
		MessageBoxA(NULL, "Box couldn't be initialized correctly.", "Box", 0);
		RetVal = -1;
	}*/

	FBoilerplate::Set(nullptr);
	return RetVal;
}
#endif