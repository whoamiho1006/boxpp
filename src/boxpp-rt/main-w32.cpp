#include "boilerplate.hpp"

#if PLATFORM_WINDOWS
#include <Windows.h>

using namespace boxpp_rt;
using namespace boxpp::boilerplates;

/* box runtime class for CLI */
class FBoxRuntimeExeW32 : public FBoxRuntimeCommon
{
public:
	FBoxRuntimeExeW32(HINSTANCE hInstance) {

	}

public:
	/* Get type of this runtime. */
	virtual ERuntimeType GetType() const override { return ERuntimeType::ExeW32; }
};

int APIENTRY WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	FBoxRuntimeExeW32 Runtime(hInstance);
	int RetVal = 0;

	if (bxEnterRuntime(&Runtime))
	{
		FBoilerplate::Set(FBoxBoilerplate::Get());
		bxExecRuntime(&Runtime);

		FBoilerplate::Set(nullptr);
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
	}

	return RetVal;
}
#endif