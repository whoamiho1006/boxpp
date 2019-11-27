#include "Boxjs.hpp"
#include "DukContext.hpp"

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

BOX_IMPLEMENT_MODULE(FBoxjsModule, AppModule);

void FBoxjsLoop::Init()
{
}


void FBoxjsLoop::Step()
{
	FDukContext& Context = FDukContext::Get();

	duk_eval_string(Context.GetRaw(), "test()");
	duk_eval_string(Context.GetRaw(), "test2()");
	duk_eval_string(Context.GetRaw(), "test2()");
	duk_eval_string(Context.GetRaw(), "test2()");

	bKeepLoop = false;
}

void FBoxjsLoop::Exit()
{
}
