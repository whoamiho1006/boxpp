#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>

namespace boxpp {

	/*
		Thread local storage. (RAW)
	*/
	class FRawTls
	{
	public:
		FASTINLINE FRawTls()
		{
#if PLATFORM_WINDOWS
			TlsIndex = w32_compat::TlsAlloc();
			bValid = true;
#endif
#if PLATFORM_POSIX
			bValid = !pthread_key_create(&TlsKey, nullptr);
#endif
		}

		FASTINLINE ~FRawTls()
		{
			if (bValid) {
#if PLATFORM_WINDOWS
				w32_compat::TlsFree(TlsIndex);
#endif
#if PLATFORM_POSIX
				pthread_key_delete(TlsKey);
#endif
			}
		}

	public:
		FASTINLINE bool Set(const void* Value) {
			if (bValid) {
#if PLATFORM_WINDOWS
				w32_compat::TlsSetValue(TlsIndex, (void*)Value);
#endif
#if PLATFORM_POSIX
				pthread_setspecific(TlsKey, Value);
#endif
				return true;
			}

			return false;
		}

		FASTINLINE void* Get() {
			void* RetVal = nullptr;

			if (bValid) {
#if PLATFORM_WINDOWS
				RetVal = w32_compat::TlsGetValue(TlsIndex);
#endif
#if PLATFORM_POSIX
				RetVal = pthread_getspecific(TlsKey);
#endif
			}

			return RetVal;
		}

	private:
#if PLATFORM_WINDOWS
		w32_compat::DWORD TlsIndex;
#endif
#if PLATFORM_POSIX
		pthread_key_t TlsKey;
#endif
		bool bValid;
	};

}