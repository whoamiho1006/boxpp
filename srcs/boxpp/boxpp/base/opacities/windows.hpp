#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

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

		BOX_COMPAT_WIN32_TYPE(CHAR, s8);
		BOX_COMPAT_WIN32_TYPE(SHORT, s16);
		BOX_COMPAT_WIN32_TYPE(INT, s32);

		BOX_COMPAT_WIN32_TYPE(WCHAR, boxpp::__unicode_supports__::wide_t);

		typedef DWORD UINT, *LPUINT;

		typedef CHAR*			LPSTR;
		typedef const CHAR*		LPCSTR;

		typedef WCHAR*			LPWSTR;
		typedef const WCHAR*	LPCWSTR;

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
		typedef int HFILE, BOOL, * LPBOOL;

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

		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	GetCurrentProcessId(void);
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL	GetWindowThreadProcessId(HWND, LPDWORD);
		NO_MANGLED BOXIMPORT HWND BOX_STDCALL	GetConsoleWindow(void);

		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL IsDebuggerPresent(void);
		NO_MANGLED BOXIMPORT void BOX_STDCALL DebugBreak(void);

		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL GetModuleFileNameA(HMODULE, LPSTR, DWORD);
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL GetModuleFileNameW(HMODULE, LPWSTR, DWORD);

		FASTINLINE DWORD GetModuleFileName(HMODULE a, LPSTR b, DWORD c)
		{
			return GetModuleFileNameA(a, b, c);
		}

		FASTINLINE DWORD GetModuleFileName(HMODULE a, LPWSTR b, DWORD c)
		{
			return GetModuleFileNameW(a, b, c);
		}

		NO_MANGLED BOXIMPORT HMODULE BOX_STDCALL LoadLibraryA(LPCSTR);
		NO_MANGLED BOXIMPORT HMODULE BOX_STDCALL LoadLibraryW(LPCWSTR);

		typedef INT_PTR(BOX_STDCALL *FARPROC)();
		NO_MANGLED BOXIMPORT FARPROC BOX_STDCALL GetProcAddress(HMODULE, LPCSTR);

		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL FreeLibrary(HMODULE);

		/* -- Windows String Conversion API -- */
		static constexpr UINT COMPAT_CP_ACP = 0;
		static constexpr UINT COMPAT_CP_UTF8 = 0;

		NO_MANGLED BOXIMPORT INT BOX_STDCALL MultiByteToWideChar(UINT, DWORD, LPCSTR, INT, LPWSTR, INT);
		NO_MANGLED BOXIMPORT INT BOX_STDCALL WideCharToMultiByte(UINT, DWORD, LPCWSTR, INT, LPSTR, INT, LPCSTR, LPBOOL);

		NO_MANGLED BOXIMPORT int BOX_STDCALL MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
		NO_MANGLED BOXIMPORT int BOX_STDCALL MessageBoxW(HWND, LPCWSTR, LPCWSTR, UINT);

		FASTINLINE int MessageBox(HWND a, LPCSTR b, LPCSTR c, UINT d)
		{
			return MessageBoxA(a, b, c, d);
		}

		FASTINLINE int MessageBox(HWND a, LPCWSTR b, LPCWSTR c, UINT d)
		{
			return MessageBoxW(a, b, c, d);
		}

		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL GetConsoleMode(HANDLE, LPDWORD);
		NO_MANGLED BOXIMPORT HANDLE BOX_STDCALL GetStdHandle(DWORD);

		static constexpr DWORD COMPAT_STD_INPUT_HANDLE = 0xFFFFFFF6;
		static constexpr DWORD COMPAT_STD_OUTPUT_HANDLE = 0xFFFFFFF5;
		static constexpr DWORD COMPAT_STD_ERROR_HANDLE = 0xFFFFFFF4;

		typedef struct _COORD {
			SHORT X;
			SHORT Y;
		} COORD, *PCOORD;

		typedef struct _SMALL_RECT {
			SHORT Left;
			SHORT Top;
			SHORT Right;
			SHORT Bottom;
		} SMALL_RECT;

		typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
			COORD      dwSize;
			COORD      dwCursorPosition;
			WORD       wAttributes;
			SMALL_RECT srWindow;
			COORD      dwMaximumWindowSize;
		} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;

		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL GetConsoleScreenBufferInfo(
			HANDLE, PCONSOLE_SCREEN_BUFFER_INFO);

		typedef struct _SYSTEMTIME {
			WORD wYear;
			WORD wMonth;
			WORD wDayOfWeek;
			WORD wDay;
			WORD wHour;
			WORD wMinute;
			WORD wSecond;
			WORD wMilliseconds;
		} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

		NO_MANGLED BOXIMPORT void BOX_STDCALL GetLocalTime(LPSYSTEMTIME lpSystemTime);

		/* Thread Locals. */
		NO_MANGLED BOXIMPORT DWORD BOX_STDCALL TlsAlloc();
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL TlsFree(DWORD);
		NO_MANGLED BOXIMPORT void* BOX_STDCALL TlsGetValue(DWORD);
		NO_MANGLED BOXIMPORT BOOL BOX_STDCALL TlsSetValue(DWORD, void*);

		/* WinSock API. */
		typedef struct WSAData {
			WORD                    _1;
			WORD                    _2;
			unsigned short          _3;
			unsigned short          _4;
			char *					_5;
			char                    _6[256 + 1];
			char                    _7[128 + 1];
		} WSADATA, *LPWSADATA;

		NO_MANGLED BOXIMPORT s32 BOX_STDCALL WSAStartup(WORD, LPWSADATA);
		NO_MANGLED BOXIMPORT s32 BOX_STDCALL WSACleanup(void);
		NO_MANGLED BOXIMPORT s32 BOX_STDCALL WSAGetLastError(void);
	}
}
#endif
