#pragma once
#include <boxpp.hpp>

#include "duktape/duktape.h"

class FDukContext
{
public:
	static FDukContext& Get();

private:
	static void OnFatalError(void* uData, const char* Msg);

private:
	FDukContext();
	~FDukContext();

public:
	FASTINLINE duk_context* GetRaw() const { return Context; }


private:
	duk_context* Context;
};