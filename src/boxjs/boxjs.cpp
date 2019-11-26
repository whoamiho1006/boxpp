#include "Boxjs.hpp"

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

BOX_IMPLEMENT_MODULE(FBoxjsModule, AppModule);

void FBoxjsLoop::Init()
{
}


void FBoxjsLoop::Step()
{
	bKeepLoop = false;
}

void FBoxjsLoop::Exit()
{
}
