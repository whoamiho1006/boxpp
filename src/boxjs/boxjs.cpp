#include "Boxjs.hpp"
#include "DukContext.hpp"
#include "DukFunction.hpp"

#include <boxpp/String.hpp>
#include <boxpp/core/Path.hpp>
#include <boxpp/core/Logging.hpp>

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

using namespace boxpp;

extern void internal_duk_require(duk_context* ctx, const char* FileName);

BOX_IMPLEMENT_MODULE(FBoxjsModule, AppModule);

void FBoxjsLoop::Init()
{
	FDukContext& Context = FDukContext::Get();
	boxpp::IApplication* App = boxpp::IApplication::Get();
	const TArray<FAnsiString>& Arguments = App->GetArguments();

	// define `args` on global context..
	duk_push_array(Context.GetRaw());

	for (s32 i = 0; i < Arguments.GetSize(); i++) {
		duk_push_string(Context.GetRaw(), Arguments[i].GetRaw());
		duk_put_prop_index(Context.GetRaw(), -2, i);
	}

	duk_put_global_string(Context.GetRaw(), "args");

	// put all arguments on `args` array.
	if (Arguments.GetSize() > 1) {
		internal_duk_require(Context.GetRaw(), Arguments[1].GetRaw());
	}
	else {
		BX_LOG(Fatal, LogCore, BOXTEXT("usage: boxjs [SCRIPT] <... VA ARGS ...>."));
	}
}

void FBoxjsLoop::Step()
{
	FDukContext& Context = FDukContext::Get();

	// find `main` function to call.
	duk_push_global_object(Context.GetRaw());
	duk_get_prop_string(Context.GetRaw(), -1, "main");

	// if there is no main function defined,
	if (duk_is_null_or_undefined(Context.GetRaw(), -1)) {
		duk_pop_n(Context.GetRaw(), 2);

		// make this to be fatal error.
		BX_LOG(Fatal, LogCore, BOXTEXT("not found: function main(...)."));
		bKeepLoop = false;
		return;
	}

	// invoke `main` function.
	duk_pop_n(Context.GetRaw(), 2);
	duk_eval_string(Context.GetRaw(), "main(args)");

	bKeepLoop = false;
}

void FBoxjsLoop::Exit()
{
}
