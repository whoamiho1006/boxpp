#include "boilerplate.hpp"
#include <boxpp/core/Logging.hpp>

#if PLATFORM_WINDOWS
#include <Windows.h>

using namespace boxpp;
using namespace boxpp::boilerplates;

extern int commonMain();

int APIENTRY WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	FBoilerplate::Set(FBoxBoilerplate::Get());
	FLogging::Get().SetUnderCLI(false);
	int RetVal = commonMain();
	FBoilerplate::Set(nullptr);
	return RetVal;
}
#endif