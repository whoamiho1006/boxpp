#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/opacities/posix.hpp>
#include <boxpp/base/opacities/windows.hpp>

#include <boxpp/base/tpls/strings/String.hpp>

namespace boxpp
{
	class FLibrary
	{
	public:
		FASTINLINE FLibrary(nullptr_t = nullptr)
		{
#if PLATFORM_WINDOWS
			hLibrary = nullptr;
#endif
#if PLATFORM_POSIX
			DlHandle = nullptr;
#endif
		}

		FASTINLINE FLibrary(const ansi_t* InPath)
		{
#if PLATFORM_WINDOWS
			hLibrary = w32_compat::LoadLibraryA(InPath);
#endif
#if PLATFORM_POSIX
			DlHandle = dlopen(InPath, 0);
#endif
		}

		FASTINLINE FLibrary(const wide_t* InPath)
		{
#if PLATFORM_WINDOWS
			hLibrary = w32_compat::LoadLibraryW(InPath);
#endif
#if PLATFORM_POSIX
			FAnsiString Ansi = InPath;
			DlHandle = dlopen(*Ansi, 0);
#endif
		}

		FASTINLINE FLibrary(FLibrary&& Other)
		{
#if PLATFORM_WINDOWS
			hLibrary = nullptr;
			Swap(hLibrary, Other.hLibrary);
#endif
#if PLATFORM_POSIX
			DlHandle = nullptr;
			Swap(DlHandle, Other.DlHandle);
#endif
		}

		FASTINLINE ~FLibrary()
		{
#if PLATFORM_WINDOWS
			if (hLibrary) {
				w32_compat::FreeLibrary(hLibrary);
			}
#endif
#if PLATFORM_POSIX
			if (DlHandle) {
				dlclose(DlHandle);
			}
#endif
		}

		FLibrary(const FLibrary&) = delete;

	public:
		FASTINLINE operator bool() const {
			bool RetVal = false;

#if PLATFORM_WINDOWS
			RetVal = (hLibrary != nullptr);
#endif
#if PLATFORM_POSIX
			RetVal = (DlHandle != nullptr);
#endif

			return RetVal;
		}

		FASTINLINE bool operator !() const { return !((bool)*this); }

	public:
		FLibrary& operator =(const FLibrary&) = delete;
		FASTINLINE FLibrary& operator =(FLibrary&& Other) {
#if PLATFORM_WINDOWS
			Swap(hLibrary, Other.hLibrary);
#endif
#if PLATFORM_POSIX
			Swap(DlHandle, Other.DlHandle);
#endif
			return *this;
		}

	public:
		FASTINLINE void* FindSymbol(const ansi_t* Symbol) const {
			void* RetVal = nullptr;

			if (*this) {
#if PLATFORM_WINDOWS
				RetVal = w32_compat::GetProcAddress(hLibrary, Symbol);
#endif
#if PLATFORM_POSIX
				RetVal = dlsym(DlHandle, Symbol);
#endif
			}

			return RetVal;
		}

		FASTINLINE void* FindSymbol(const wide_t* Symbol) const {
			FAnsiString Ansi = Symbol;
			return FindSymbol(*Ansi);
		}

	private:
#if PLATFORM_WINDOWS
		mutable w32_compat::HMODULE hLibrary;
#endif
#if PLATFORM_POSIX
		mutable void* DlHandle;
#endif
	};

}