#include "loader.hpp"

/*
#if PLATFORM_WINDOWS
#include <Windows.h>
#endif
*/
#if PLATFORM_POSIX
/*	Auto-completion block for Visual-studio. 
	Auto-completion blocks will not be really compiled. */
/*#	if BOX_NOT_COMPILED
		extern void* dlopen(const char *filename, int flag);
		extern void* dlsym(void *handle, const char *symbol);
		extern int dlclose(void *handle);
#		define RTLD_NOW		0
#	else
#		include <dlfcn.h>
#	endif*/
#endif

/*
namespace boxpp_rt {
	template<typename FAR_Type>
	FASTINLINE FAR_Type FindBoxProc(const SBoxInterface& Interface, const boxpp::c8* Symbol) {
		if (Interface.Name && Interface.Handle) {
#if PLATFORM_WINDOWS
			return (FAR_Type)GetProcAddress((HMODULE)Interface.Handle, Symbol);
#endif
#if PLATFORM_POSIX
			return (FAR_Type)dlsym(Interface.Handle, Symbol);
#endif
		}

		return nullptr;
	}

	bool TryLoadBox(SBoxInterface& Interface, const boxpp::c8* Name) {
#if PLATFORM_WINDOWS
		Interface.Handle = (OBoxLibrary)LoadLibraryA(Interface.Name = Name);
#endif
#if PLATFORM_POSIX
		Interface.Handle = (OBoxLibrary)dlopen(Interface.Name = Name, RTLD_NOW);
#endif

		if (Interface.Handle != nullptr) 
		{
			Interface.Enter = FindBoxProc<FAR_bxEnterRuntime>(Interface, SYMBOL_bxEnterRuntime);
			Interface.Leave = FindBoxProc<FAR_bxLeaveRuntime>(Interface, SYMBOL_bxLeaveRuntime);
			Interface.Exec = FindBoxProc<FAR_bxExecRuntime>(Interface, SYMBOL_bxExecRuntime);

#if PLATFORM_WINDOWS
			Interface.Enter = Interface.Enter ? Interface.Enter :
				FindBoxProc<FAR_bxEnterRuntime>(Interface, SYMBOL_bxEnterRuntime_Mangled);

			Interface.Leave = Interface.Leave ? Interface.Leave :
				FindBoxProc<FAR_bxLeaveRuntime>(Interface, SYMBOL_bxLeaveRuntime_Mangled);

			Interface.Exec = Interface.Exec ? Interface.Exec :
				FindBoxProc<FAR_bxExecRuntime>(Interface, SYMBOL_bxExecRuntime_Mangled);
#endif
#if PLATFORM_POSIX
			Interface.Enter = Interface.Enter ? Interface.Enter :
				FindBoxProc<FAR_bxEnterRuntime>(Interface,"_" SYMBOL_bxEnterRuntime);

			Interface.Leave = Interface.Leave ? Interface.Leave :
				FindBoxProc<FAR_bxLeaveRuntime>(Interface, "_" SYMBOL_bxLeaveRuntime);

			Interface.Exec = Interface.Exec ? Interface.Exec :
				FindBoxProc<FAR_bxExecRuntime>(Interface, "_" SYMBOL_bxExecRuntime);
#endif

			if (!Interface.Enter || !Interface.Leave) {
				bxUnloadInterface(Interface);
				return false;
			}

			return true;
		}

		Interface.Name = nullptr;
		return false;
	}


	bool bxLoadInterface(SBoxInterface& Interface) {
		if (TryLoadBox(Interface, SHARED_bxVersionedLibrary) ||
			TryLoadBox(Interface, SHARED_bxLibrary))
		{
			return true;
		}

		return false;
	}

	void bxUnloadInterface(SBoxInterface& Interface) {
		if (Interface.Name && Interface.Handle) {
#if PLATFORM_WINDOWS
			FreeLibrary((HMODULE)Interface.Handle);
#endif
#if PLATFORM_POSIX
			dlclose(Interface.Handle);
#endif
		}

		::memset(&Interface, 0, sizeof(Interface));
	}
}*/