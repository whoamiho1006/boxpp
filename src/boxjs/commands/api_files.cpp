#include "../DukFunction.hpp"
#include "../DukContext.hpp"

#include <boxpp/String.hpp>
#include <boxpp/core/Path.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;

/* files.open(name, mode). (returns fp) */
DUK_DEFINE_SCOPED_FUNCTION(files, open, 2) {
	FILE* fp = fopen(duk_to_string(Context, 0), duk_to_string(Context, 1));

	if (fp) {
		duk_push_object(Context);

		duk_push_pointer(Context, fp);
		duk_put_prop_string(Context, -2, "__raw");

		duk_push_string(Context, "files:private");
		duk_put_prop_string(Context, -2, "__raw_magic");

		return EDukRet::WithResult;
	}

	return EDukRet::Undefined;
}

/* files.close(fp). */
DUK_DEFINE_SCOPED_FUNCTION(files, close, 1) {
	duk_get_prop_string(Context, 0, "__raw_magic");
	
	if (strcmp(duk_to_string(Context, -1), "files:private")) {
		duk_pop(Context);
		duk_push_boolean(Context, false);
		return EDukRet::WithResult;
	}

	duk_pop(Context);
	duk_get_prop_string(Context, 0, "__raw");

	FILE* fp = (FILE*) duk_get_pointer(Context, -1);
	duk_pop(Context);

	if (fp) {
		duk_push_pointer(Context, nullptr);
		duk_put_prop_string(Context, 0, "__raw");

		fclose(fp);
		duk_push_boolean(Context, true);
	}

	else {
		duk_push_boolean(Context, false);
	}

	return EDukRet::WithResult;
}

DUK_DEFINE_SCOPED_FUNCTION(files, readLine, 1) {
	TArray<ansi_t> Buffer;

	duk_get_prop_string(Context, 0, "__raw_magic");

	if (strcmp(duk_to_string(Context, -1), "files:private")) {
		duk_pop(Context);
		duk_push_boolean(Context, false);
		return EDukRet::WithResult;
	}

	duk_pop(Context);
	duk_get_prop_string(Context, 0, "__raw");

	FILE* fp = (FILE*)duk_get_pointer(Context, -1);
	duk_pop(Context);

	if (fp) {
		ansi_t Buf = 0;

		while (fread(&Buf, 1, sizeof(ansi_t), fp) > 0) {
			if (Buf == '\n') {
				break;
			}

			else if (Buf != '\r')
				Buffer.Add(Buf);
		}

		if (Buffer.GetSize() > 0) {
			Buffer.Add(0);
			duk_push_string(Context, Buffer.GetRaw());
			return EDukRet::WithResult;
		}
	}

	return EDukRet::Undefined;
}

DUK_DEFINE_SCOPED_FUNCTION(files, writeLine, 2) {
	FAnsiString Str = duk_to_string(Context, 1);
	duk_get_prop_string(Context, 0, "__raw_magic");

	if (strcmp(duk_to_string(Context, -1), "files:private")) {
		duk_pop(Context);
		duk_push_boolean(Context, false);
		return EDukRet::WithResult;
	}

	duk_pop(Context);
	duk_get_prop_string(Context, 0, "__raw");

	FILE* fp = (FILE*)duk_get_pointer(Context, -1);
	duk_pop(Context);

	if (fp) {

		if (!Str.EndsWith("\n")) {
			Str.Append("\n");
		}

		if (fwrite(Str.GetRaw(), 1, Str.GetSize(), fp) > 0) {
			duk_push_boolean(Context, true);
			return EDukRet::WithResult;
		}
	}

	return EDukRet::Undefined;
}