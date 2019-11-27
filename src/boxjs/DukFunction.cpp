#include "DukFunction.hpp"
#include "DukContext.hpp"

using namespace boxpp;

FDukFunction::FDukFunction(const char* Name, boxpp::s32 Args) {
	FDukContext& Context = FDukContext::Get();
	duk_context* Raw = Context.GetRaw();

	duk_push_c_function(Raw, FuncProxy, Args);
	
	duk_push_pointer(Raw, this);
	duk_put_prop_string(Raw, -2, "__raw");

	duk_put_global_string(Raw, Name);
}

void duk_define_global(duk_context* Ctxt, const char* Object) {
	FAnsiString Scope, Next(Object);

	while (Next.GetSize()) {
		if (Scope.GetSize()) {
			duk_push_string(Ctxt, Scope.GetRaw());
			duk_eval(Ctxt);
		}
		else {
			duk_push_global_object(Ctxt);
		}

		s32 Offset = Next.IndexOf('.');
		FAnsiString Current;

		if (Offset >= 0) {
			Current.Append(*Next, Offset);
			Next.RemoveAt(0, Offset + 1);
		}
		else {
			Current = Next;
			Next.Clear();
		}

		duk_get_prop_string(Ctxt, -1, *Current);
		if (duk_is_undefined(Ctxt, -1)) {
			duk_pop(Ctxt);

			duk_push_object(Ctxt);
			duk_put_prop_string(Ctxt, -2, *Current);
		}

		duk_pop(Ctxt);

		if (Scope.GetSize()) {
			Scope.Append('.');
		}

		Scope.Append(Current);
	}
}

FDukFunction::FDukFunction(const char* Object, const char* Name, boxpp::s32 Args)
{
	FDukContext& Context = FDukContext::Get();
	duk_context* Raw = Context.GetRaw();

	// define object scope.
	duk_define_global(Raw, Object);

	// acquire object scope.
	duk_push_string(Raw, Object);
	duk_eval(Raw);
	
	// define function into object.
	duk_push_c_function(Raw, FuncProxy, Args);

	duk_push_pointer(Raw, this);
	duk_put_prop_string(Raw, -2, "__raw");

	duk_put_prop_string(Raw, -2, Name);

	// pop object scope.
	duk_pop(Raw);
}

FDukFunction::~FDukFunction() {

}

duk_ret_t FDukFunction::FuncProxy(duk_context* Context)
{
	void* Raw = nullptr;
	FDukFunction* Func = nullptr;

	duk_push_current_function(Context);
	duk_get_prop_string(Context, -1, "__raw");

	if ((Raw = duk_get_pointer(Context, -1)) != nullptr) {
		Func = (FDukFunction*)Raw;
	}

	duk_pop(Context);

	if (Func) {
		return Func->OnInvoke(Context) == EDukRet::Undefined ? 0 : 1;
	}

	return 0;
}
