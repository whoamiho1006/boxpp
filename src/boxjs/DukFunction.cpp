#include "DukFunction.hpp"
#include "DukContext.hpp"

FDukFunction::FDukFunction(const char* Name, boxpp::s32 Args) {
	FDukContext& Context = FDukContext::Get();
	duk_context* Raw = Context.GetRaw();

	duk_push_c_function(Raw, FuncProxy, Args);
	
	duk_push_pointer(Raw, this);
	duk_put_prop_string(Raw, -2, "__raw");

	duk_put_global_string(Raw, Name);
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

DUK_DEFINE_FUNCTION(test, 0) {
	printf("hello!\n");

	
	return EDukRet::Undefined;
}

class test2_data
{
public:
	int counter;

public:
	void test() {
		printf("TTT");
	}
};

DUK_DEFINE_FUNCTION_WITH_USERDATA(test2, 0, test2_data) {
	Data.test();
	printf("counter: %d\n", ++Data.counter);
	return EDukRet::Undefined;
}