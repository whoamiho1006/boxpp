#ifndef __BOXPP_HINTS_WINDOWS_HPP__
#define __BOXPP_HINTS_WINDOWS_HPP__

/*
	Windows Opacity header.
*/

#if PLATFORM_WINDOWS
#define BOX_COMPAT_WIN32_TYPE(Name, Decl) \
	typedef Decl Name; typedef Name* LP##Name

#define BOX_COMPAT_WIN32_HANDLE(Name) \
	typedef struct { boxpp::s32 __COMPAT_HANDLE__; }* Name

namespace boxpp {
	namespace w32_compat {
		/* -- Windows compatible types. -- */
		typedef void* LPVOID;

		typedef ssize_t INT_PTR, *PINT_PTR;
		typedef size_t UINT_PTR, *PUINT_PTR;

		typedef ssize_t LONG_PTR, *PLONG_PTR;
		typedef size_t ULONG_PTR, *PULONG_PTR;

		BOX_COMPAT_WIN32_TYPE(BYTE, u8);
		BOX_COMPAT_WIN32_TYPE(WORD, u16);
		BOX_COMPAT_WIN32_TYPE(DWORD, u32);

		BOX_COMPAT_WIN32_TYPE(CHAR, s16);
		BOX_COMPAT_WIN32_TYPE(SHORT, s16);
		BOX_COMPAT_WIN32_TYPE(INT, s32);

		BOX_COMPAT_WIN32_TYPE(WCHAR, boxpp::__unicode_supports__::wide_t);

		typedef LPCHAR			LPSTR;
		typedef const LPCHAR	LPCSTR;

		typedef WCHAR*			LPWSTR;
		typedef const LPWSTR	LPCWSTR;

		typedef LPVOID HANDLE;
		typedef HANDLE HGLOBAL;
		BOX_COMPAT_WIN32_HANDLE(HINSTANCE);
		typedef HINSTANCE HMODULE;

		BOX_COMPAT_WIN32_HANDLE(HWND);
		BOX_COMPAT_WIN32_HANDLE(HDC);
		BOX_COMPAT_WIN32_HANDLE(HICON);
		BOX_COMPAT_WIN32_HANDLE(HMENU);
		BOX_COMPAT_WIN32_HANDLE(HGDIOBJ);
		BOX_COMPAT_WIN32_HANDLE(HPEN);
		BOX_COMPAT_WIN32_HANDLE(HBRUSH);
		BOX_COMPAT_WIN32_HANDLE(HBITMAP);

		typedef UINT_PTR WPARAM;
		typedef LONG_PTR LPARAM, LRESULT;
		typedef int HFILE, BOOL;

		typedef ULONG_PTR SIZE_T, *PSIZE_T;
		typedef LONG_PTR SSIZE_T, *PSSIZE_T;

		/* -- Windows Critical Section API. -- */
		typedef struct CRITICAL_SECTION {
			/* WIN32: 24 Bytes. WIN64: 40 bytes. */
			LPVOID _1[1]; DWORD _2[2]; LPVOID _3[3];
		}* LPCRITICAL_SECTION;

		NO_MANGLED BOXIMPORT void BOX_STDCALL	InitializeCriticalSection(LPCRITICAL_SECTION);
		NO_MANGLED BOXIMPORT void BOX_STDCALL	DeleteCriticalSection(LPCRITICAL_SECTION);
		NO_MANGLED BOXIMPORT INT  BOX_STDCALL	TryEnterCriticalSection(LPCRITICAL_SECTION);
		NO_MANGLED BOXIMPORT void BOX_STDCALL	EnterCriticalSection(LPCRITICAL_SECTION);
		NO_MANGLED BOXIMPORT void BOX_STDCALL	LeaveCriticalSection(LPCRITICAL_SECTION);

		typedef struct _SECURITY_ATTRIBUTES {
			DWORD _1; LPVOID _2; BOOL _3;
		} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

		static constexpr DWORD COMPAT_Infinite = 0xFFFFFFFFu;

		/* -- Windows Handle API -- */
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL	CloseHandle(HANDLE);
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	GetLastError(void);

		/* -- Windows Waitable Series API -- */
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	WaitForSingleObject(HANDLE, DWORD);
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	WaitForMultipleObjects(DWORD, const HANDLE*, BOOL, DWORD);
		NO_MANGLED BOXIMPORT HANDLE BOX_STDCALL	CreateEventA(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR);
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL	SetEvent(HANDLE);
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL	ResetEvent(HANDLE);

		/* -- Windows Thread API -- */
		typedef DWORD(BOX_STDCALL *LPTHREAD_START_ROUTINE)(LPVOID);
		NO_MANGLED BOXIMPORT void BOX_STDCALL	Sleep(DWORD);
		NO_MANGLED BOXIMPORT HANDLE BOX_STDCALL	GetCurrentThread(void);
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	GetCurrentThreadId(void);
		NO_MANGLED BOXIMPORT HANDLE BOX_STDCALL	CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL	TerminateThread(HANDLE, DWORD);
		NO_MANGLED BOXIMPORT void BOX_STDCALL	ExitThread(DWORD);
	}
}
#endif

#endif // !__BOXPP_HINTS_WINDOWS_HPP__