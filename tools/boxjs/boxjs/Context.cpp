#include "Context.hpp"
#include <stdio.h>

FContext& FContext::Get() {
	static FContext Context;
	return Context;
}

void FContext::FatalProxy(void * uData, const char * Msg)
{
	((FContext*)uData)->OnFatalError(Msg);
}

FContext::FContext()
	: bFatalError (false)
{
	Context = duk_create_heap_default();
}

FContext::~FContext()
{
	if (Context) {
		duk_destroy_heap(Context);
	}
}

void FContext::Evaluate(const char* Scripts)
{
	char* Buffer = nullptr;
	FILE* fp = fopen(Scripts, "r+");
	int reads = 0;
	

}

int FContext::Execute()
{


	return bFatalError ? EXIT_FAILURE : EXIT_SUCCESS;
}

void FContext::OnFatalError(const char* Msg)
{
	fprintf(stderr, "fatal: %s\n", Msg);
	bFatalError = true;
}
