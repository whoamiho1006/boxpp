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
	char Buffer[1024] = { 0, };
	char* CombinedBuffer = nullptr;
	FILE* File = fopen(Scripts, "r+");
	int Reads = 0, TotalBytes = 0;
	
	if (!File) {
		fprintf(stderr, "fatal: not found: %s\n", Scripts);
		bFatalError = true;
		return;
	}

	while ((Reads = fread(File, 1, sizeof(Buffer), File)) > 0) {
		if (!CombinedBuffer) {
			CombinedBuffer = (char*) malloc((TotalBytes += Reads) + 1);
		}

		else {
			CombinedBuffer = (char*)realloc(CombinedBuffer, (TotalBytes += Reads) + 1);
		}

		memcpy(CombinedBuffer + TotalBytes - Reads, Buffer, Reads);
	}


	if (CombinedBuffer) {
		fprintf(stderr, "evaluating scripts... (%s)\n", Scripts);

		CombinedBuffer[TotalBytes] = 0;
		duk_push_string(Context, CombinedBuffer);
		duk_eval(Context);

		free(CombinedBuffer);
	}

	fclose(File);
}

int FContext::Execute(int Argc, char** Argv)
{
	duk_idx_t Target = duk_push_array(Context);

	for (int i = 0; i < Argc; i++) {
		duk_push_string(Context, Argv[i]);
		duk_put_prop_index(Context, Target, i);
	}

	duk_put_global_string(Context, "argv");
	duk_push_int(Context, Argc);

	duk_put_global_string(Context, "argc");

	return bFatalError ? EXIT_FAILURE : EXIT_SUCCESS;
}

void FContext::AddFunction(const char * Name, duk_ret_t(*Func)(duk_context *), int Argc)
{

	duk_push_c_function(Context, Func, Argc);
	//duk_push
}

void FContext::OnFatalError(const char* Msg)
{
	fprintf(stderr, "fatal: %s\n", Msg);
	bFatalError = true;
}
