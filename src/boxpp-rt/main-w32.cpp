#include <boxpp/core/Logging.hpp>

#if PLATFORM_WINDOWS
#include <Windows.h>

using namespace boxpp;

extern int commonMain();

int APIENTRY WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	FLogging::Get().SetUnderCLI(false);
	int RetVal = commonMain();
	return RetVal;
}
#endif