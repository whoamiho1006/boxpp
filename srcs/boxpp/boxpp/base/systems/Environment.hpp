#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>

#include <boxpp/base/tpls/strings/String.hpp>

namespace boxpp {

	class BOXPP FEnvironment
	{
	public:
		FASTINLINE static bool Get(const ansi_t* Name, ansi_t* OutValue, size_t MaxSize) {
			if (Name) {
#if PLATFORM_WINDOWS
				if (w32_compat::GetEnvironmentVariable(Name, OutValue, MaxSize)) {
					return true;
				}

				else if (w32_compat::GetLastError() !=
					w32_compat::COMPAT_ERROR_ENVVAR_NOT_FOUND)
				{
					return true;
				}
#else
				if (const ansi_t* Value = ::getenv(Name)) {
					TNativeString<ansi_t>::Strncpy(OutValue, Value, MaxSize);
					return true;
				}
#endif
			}

			return false;
		}

		FASTINLINE static bool Set(const ansi_t* Name, const ansi_t* Value) {
			bool RetVal = false;

#if PLATFORM_WINDOWS
			RetVal = w32_compat::SetEnvironmentVariable(Name, Value);
#else
			RetVal = !::setenv(Name, Value, 1);		
#endif

			return RetVal;
		}
	};
} 