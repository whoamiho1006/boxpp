#include "../DukFunction.hpp"
#include "../DukContext.hpp"

#include <boxpp/String.hpp>
#include <boxpp/core/Path.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;

DUK_DEFINE_SCOPED_FUNCTION(console, log, 1) {
	BX_LOG(Note, LogCore, BOXTEXT("console.log: %S"), duk_to_string(Context, 0));
	return EDukRet::Undefined;
}

DUK_DEFINE_SCOPED_FUNCTION(console, warn, 1) {
	BX_LOG(Warning, LogCore, BOXTEXT("console.warn: %S"), duk_to_string(Context, 0));
	return EDukRet::Undefined;
}

DUK_DEFINE_SCOPED_FUNCTION(console, error, 1) {
	BX_LOG(Error, LogCore, BOXTEXT("console.error: %S"), duk_to_string(Context, 0));
	return EDukRet::Undefined;
}

DUK_DEFINE_SCOPED_FUNCTION(console, fatal, 1) {
	BX_LOG(Fatal, LogCore, BOXTEXT("console.fatal: %S"), duk_to_string(Context, 0));
	return EDukRet::Undefined;
}