#include "DukContext.hpp"

#include <boxpp/core/Logging.hpp>

FDukContext& FDukContext::Get() {
	static FDukContext Duk;
	return Duk;
}

void FDukContext::OnFatalError(void* uData, const char* Msg)
{
	BX_LOG(Fatal,
		boxpp::LogCore,
		BOXTEXT("boxjs: %S"),
		Msg);

	exit(-1);
}

FDukContext::FDukContext() {
	Context = duk_create_heap(0, 0, 0, this, OnFatalError);
}

FDukContext::~FDukContext() {
	if (Context) {
		duk_destroy_heap(Context);
	}
}
