#include "../DukFunction.hpp"
#include "../DukContext.hpp"

#include <boxpp/String.hpp>
#include <boxpp/core/Path.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;

class FDukApiRequire
{
public:
	FASTINLINE bool FindScriptAtBinary(const FAnsiString& Name, FAnsiString& FoundScript) {
		if (Name.EndsWith(".js")) {
			FAnsiString AbsolutePath;

			FPath::Binaries(AbsolutePath);
			AbsolutePath.Append("/scripts/");
			AbsolutePath.Append(Name);

			if (FPath::FileExists(*AbsolutePath)) {
				FoundScript = AbsolutePath;
				return true;
			}
		}

		return false;
	}

	FASTINLINE bool FindScriptAtCwd(const FAnsiString& Name, FAnsiString& FoundScript) {
		if (Name.EndsWith(".js")) {
			FAnsiString AbsolutePath;

			FPath::Binaries(AbsolutePath);
			AbsolutePath.Append("/");
			AbsolutePath.Append(Name);

			if (FPath::FileExists(*AbsolutePath)) {
				FoundScript = AbsolutePath;
				return true;
			}

			FPath::Binaries(AbsolutePath);
			AbsolutePath.Append("/scripts/");
			AbsolutePath.Append(Name);

			if (FPath::FileExists(*AbsolutePath)) {
				FoundScript = AbsolutePath;
				return true;
			}
		}

		return false;
	}

	FASTINLINE bool FindScript(const FAnsiString& Name, FAnsiString& FoundScript) {
		if (!Name.EndsWith(".js")) {
			FAnsiString Modified = Name;
			Modified.Append(".js");
			return FindScript(Modified, FoundScript);
		}

		else if (FPath::FileExists(*Name)) {
			FoundScript = Name;
			return true;
		}

		else if (
			FindScriptAtBinary(Name, FoundScript) ||
			FindScriptAtCwd(Name, FoundScript))
			return true;

		return false;
	}

	FASTINLINE bool LoadScript(const FAnsiString& Name, TArray<ansi_t>& Buffer) {
		FILE* fp = fopen(*Name, "r+");
		ansi_t Temp[1024];
		s32 Reads = 0;

		if (!fp) {
			return false;
		}

		while ((Reads = fread(Temp, 1, sizeof(Temp), fp)) > 0) {
			Buffer.Append(Temp, Reads);
		}

		Buffer.Add(0);
		fclose(fp);

		return true;
	}

};

DUK_DEFINE_FUNCTION_WITH_USERDATA(require, 1, FDukApiRequire) {
	FAnsiString Script = duk_to_string(Context, 0);
	FAnsiString FoundScript;

	BX_LOG(Note, LogCore, BOXTEXT("require: %S"), *Script);

	if (Data.FindScript(Script, FoundScript)) {
		TArray<ansi_t> ScriptData;

		// Make backup on stack of module.
		duk_push_global_object(Context);
		duk_get_prop_string(Context, -1, "module");
		duk_remove(Context, -2);

		// define new modules.export object.
		duk_push_object(Context);

		duk_push_object(Context);
		duk_put_prop_string(Context, -2, "exports");

		duk_put_global_string(Context, "module");
		
		// load script from disk.
		if (!Data.LoadScript(FoundScript, ScriptData)) {
			BX_LOG(Warning, LogCore, 
				BOXTEXT("require: load failure: %S"),
				FoundScript.GetRaw());

			return EDukRet::Undefined;
		}
		
		// compile and execute the script.
		duk_push_string(Context, FoundScript.GetRaw());
		duk_compile_string_filename(Context, DUK_COMPILE_EVAL, ScriptData.GetRaw());
		duk_call(Context, 0);

		// configure return value.
		duk_push_string(Context, "module.exports");
		duk_eval(Context);

		// restore previous module object.
		duk_dup(Context, -2);
		duk_remove(Context, -3);

		duk_put_global_string(Context, "module");
		return EDukRet::WithResult;
	}

	else {
		BX_LOG(Warning, LogCore, BOXTEXT("require: %S"), *Script);
	}

	return EDukRet::Undefined;
}

void internal_duk_require(duk_context* ctx, const char* FileName) {
	duk_push_string(ctx, "require");
	duk_eval(ctx);

	duk_push_string(ctx, FileName);
	duk_call(ctx, 1);
}