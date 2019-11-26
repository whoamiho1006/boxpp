#pragma once
#include "3rd-parties/Duktape.hpp"

class FContext
{
public:
	static FContext& Get();

private:
	static void FatalProxy(void*, const char*);

public:
	FContext();
	~FContext();

public:
	void Evaluate(const char* Scripts);
	int Execute();

private:
	void OnFatalError(const char* Msg);

private:
	duk_context* Context;
	bool bFatalError;
};