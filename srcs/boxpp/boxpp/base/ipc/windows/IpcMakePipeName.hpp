#pragma once

#if defined (IPC_IMPL_INCLUDES) && IPC_IMPL_INCLUDES
namespace boxpp {
	namespace ipc_internals {
#if PLATFORM_WINDOWS
		template<typename CharType> struct TIpcPipeNameFmt {
			static constexpr const CharType* Fmt = "\\\\.\\pipe\\boxpp-%s";
		};

		template<> struct TIpcPipeNameFmt<wide_t> {
			static constexpr const wide_t* Fmt = L"\\\\.\\pipe\\boxpp-%s";
		};
#else

		template<typename CharType> struct TIpcPipeNameFmt {
			static constexpr const CharType* Fmt = "/tmp/boxpp/%s.ipc";
		};

		template<> struct TIpcPipeNameFmt<wide_t> {
			static constexpr const wide_t* Fmt = L"/tmp/boxpp/%s.ipc";
		};
#endif
#if PLATFORM_UNIX_KIND
#define BOXPP_IPC_DIRECTORY "/tmp/boxpp"
#endif

		template<typename CharType>
		FASTINLINE void IPC_MakePipeName(CharType* Buffer, const CharType* Name) {
			u64 HashVal = internals::hashDjb2(Name, TNativeString<CharType>::Strlen(Name));
			CharType Temp[24] = { 0, };

			TNativeString<CharType>::Ltoa(HashVal, Temp);
			TVsprintf<CharType>::Sprintf(Buffer, TIpcPipeNameFmt<CharType>::Fmt, Temp);
		}
	}
}
#endif