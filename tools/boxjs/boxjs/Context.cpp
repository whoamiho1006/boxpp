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
	Context = duk_create_heap(0, 0, 0, this, FatalProxy);
	AddFunction("require", requireImpl, 1);
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

	while ((Reads = fread(Buffer, 1, sizeof(Buffer), File)) > 0) {
		if (!CombinedBuffer) {
			CombinedBuffer = (char*) malloc((TotalBytes += Reads) + 1);
		}

		else {
			CombinedBuffer = (char*)realloc(CombinedBuffer, (TotalBytes += Reads) + 1);
		}

		memcpy(CombinedBuffer + TotalBytes - Reads, Buffer, Reads);
	}

	if (CombinedBuffer) {
		//fprintf(stderr, "evaluating scripts... (%s)\n", Scripts);

		CombinedBuffer[TotalBytes] = 0;

		duk_push_string(Context, Scripts);
		duk_compile_string_filename(Context, DUK_COMPILE_EVAL, CombinedBuffer);
		duk_call(Context, 0);

		free(CombinedBuffer);
	}

	fclose(File);
}

void FContext::Require(const char* Scripts)
{
	duk_push_object(Context);
	duk_put_global_string(Context, "exports");

	Evaluate(Scripts);

	if (!bFatalError) {
		duk_push_string(Context, "exports");
		duk_eval(Context);

		duk_push_undefined(Context);
		duk_put_global_string(Context, "exports");
	}
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
	duk_eval_string_noresult(Context, "main()");
	
	return bFatalError ? EXIT_FAILURE : EXIT_SUCCESS;
}

duk_ret_t FContext::requireImpl(duk_context * Context)
{
	Get().Require(duk_to_string(Context, 0));
	if (Get().IsFatalError()) return 0;
	return 1;
}

void FContext::AddFunction(const char* Name, duk_ret_t(*Func)(duk_context *), int Argc)
{
	duk_push_c_function(Context, Func, Argc);
	duk_put_global_string(Context, Name);
}

void FContext::OnFatalError(const char* Msg)
{
	fprintf(stderr, "fatal: %s\n", Msg);
	bFatalError = true;
	exit(EXIT_FAILURE);
}
