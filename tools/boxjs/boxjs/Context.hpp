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
	void Require(const char* Scripts);
	int Execute(int Argc, char** Argv);

private:
	static duk_ret_t requireImpl(duk_context* Context);

public:
	void AddFunction(const char* Name, duk_ret_t(*Func)(duk_context*), int Argc);

private:
	void OnFatalError(const char* Msg);

public:
	bool IsFatalError() {
		return bFatalError;
	}

private:
	duk_context* Context;
	bool bFatalError;
};
