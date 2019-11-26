#pragma once
#include "Context.hpp"

class FConsole
{
public:
	FConsole();

public:
	static duk_ret_t Log(duk_context* Context);
	static duk_ret_t Warn(duk_context* Context);
	static duk_ret_t Error(duk_context* Context);

};