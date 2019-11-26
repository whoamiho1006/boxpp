#include "Console.hpp"


FConsole Console;

FConsole::FConsole()
{
	FContext::Get().AddFunction("log", Log, 1);
	FContext::Get().AddFunction("warn", Warn, 1);
	FContext::Get().AddFunction("error", Error, 1);
}

duk_ret_t FConsole::Log(duk_context* Context)
{
	fprintf(stdout, "note: %s\n", duk_get_string(Context, 0));
	return 0;
}

duk_ret_t FConsole::Warn(duk_context* Context)
{
	fprintf(stderr, "warning: %s\n", duk_get_string(Context, 0));
	return 0;
}

duk_ret_t FConsole::Error(duk_context* Context)
{
	fprintf(stderr, "error: %s\n", duk_get_string(Context, 0));
	return 0;
}
