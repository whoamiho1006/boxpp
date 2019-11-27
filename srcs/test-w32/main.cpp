#include <boxpp/base/Base.hpp>
#pragma comment(lib, "boxpp.lib")

using namespace boxpp;

NO_MANGLED int BOX_STDCALL WinMain(w32_compat::HINSTANCE, 
	w32_compat::HINSTANCE, w32_compat::LPSTR, int) 
{
	FDateTime Now = FDateTime::Now();
	FDebugger::Init();

	BOX_ASSERT(false, "");
}