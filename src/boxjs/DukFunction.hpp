#pragma once
#include <boxpp.hpp>

#include "duktape/duktape.h"

enum class EDukRet
{
	Undefined,
	WithResult
};

class FDukFunction
{
public:
	FDukFunction(const char* Name, boxpp::s32 Args);
	FDukFunction(const char* Object, const char* Name, boxpp::s32 Args);
	virtual ~FDukFunction();

private:
	static duk_ret_t FuncProxy(duk_context* Context);

protected:
	virtual EDukRet OnInvoke(duk_context* Context) {
		return EDukRet::Undefined;
	}
};

#define DUK_DEFINE_FUNCTION(Name, Args) \
	EDukRet FunkBody_##Name(duk_context* Context); \
	class FDukFunction_##Name : public FDukFunction { \
	public: FDukFunction_##Name() : FDukFunction(#Name, Args) { } \
	protected: virtual EDukRet OnInvoke(duk_context* Context) override \
		{ return FunkBody_##Name(Context); } \
	}; \
	\
	FDukFunction_##Name __FUNC_##Name;\
	EDukRet FunkBody_##Name(duk_context* Context)


#define DUK_DEFINE_FUNCTION_WITH_USERDATA(Name, Args, DataType) \
	EDukRet FunkBody_##Name(duk_context* Context, DataType& Data); \
	class FDukFunction_##Name : public FDukFunction { \
	public: FDukFunction_##Name() : FDukFunction(#Name, Args) { ::memset(&Data, 0, sizeof(DataType)); } \
	protected: virtual EDukRet OnInvoke(duk_context* Context) override \
		{ return FunkBody_##Name(Context, Data); } \
	public: DataType Data; \
	}; \
	\
	FDukFunction_##Name __FUNC_##Name;\
	EDukRet FunkBody_##Name(duk_context* Context, DataType& Data)

#define DUK_DEFINE_SCOPED_FUNCTION(Space, Name, Args) \
	EDukRet FunkBody_##Name(duk_context* Context); \
	class FDukFunction_##Name : public FDukFunction { \
	public: FDukFunction_##Name() : FDukFunction(#Space, #Name, Args) { } \
	protected: virtual EDukRet OnInvoke(duk_context* Context) override \
		{ return FunkBody_##Name(Context); } \
	}; \
	\
	FDukFunction_##Name __FUNC_##Name;\
	EDukRet FunkBody_##Name(duk_context* Context)

#define DUK_DEFINE_SCOPED_FUNCTION_WITH_USERDATA(Space, Name, Args, DataType) \
	EDukRet FunkBody_##Name(duk_context* Context, DataType& Data); \
	class FDukFunction_##Name : public FDukFunction { \
	public: FDukFunction_##Name() : FDukFunction(#Space, #Name, Args) { ::memset(&Data, 0, sizeof(DataType)); } \
	protected: virtual EDukRet OnInvoke(duk_context* Context) override \
		{ return FunkBody_##Name(Context, Data); } \
	public: DataType Data; \
	}; \
	\
	FDukFunction_##Name __FUNC_##Name;\
	EDukRet FunkBody_##Name(duk_context* Context, DataType& Data)
