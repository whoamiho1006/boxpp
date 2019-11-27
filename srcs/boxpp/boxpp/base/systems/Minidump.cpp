#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <DbgHelp.h>
#include <string>

#pragma comment(lib, "Dbghelp.lib")

LPTOP_LEVEL_EXCEPTION_FILTER MinidumpPreviousCallback = nullptr;
LONG WINAPI MinidumpReportCallback(struct _EXCEPTION_POINTERS *ExceptionInfo);

void MinidumpInstall() {
	MinidumpPreviousCallback =
		::SetUnhandledExceptionFilter(MinidumpReportCallback);
}


LONG WINAPI MinidumpReportCallback(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	CHAR FileName[FILENAME_MAX] = { 0, };
	MINIDUMP_EXCEPTION_INFORMATION Minidump;
	SYSTEMTIME SystemTime;
	HANDLE hDumpFile;

	while (true) {
		GetLocalTime(&SystemTime);

		sprintf(FileName, "%04d%02d%02d_%02d_%02d_%02d.dmp",
			SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

		hDumpFile = CreateFileA(FileName,
			GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);

		if (hDumpFile == INVALID_HANDLE_VALUE)
			break;

		Minidump.ThreadId = GetCurrentThreadId();
		Minidump.ExceptionPointers = ExceptionInfo;
		Minidump.ClientPointers = FALSE;

		if (!MiniDumpWriteDump(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hDumpFile, MINIDUMP_TYPE::MiniDumpNormal,
				&Minidump, NULL, NULL))
		{
			CloseHandle(hDumpFile);
			DeleteFileA(FileName);
		}

		else {
			CloseHandle(hDumpFile);
		}

		break;
	}
	
	return (MinidumpPreviousCallback ?
		MinidumpPreviousCallback(ExceptionInfo) :
		EXCEPTION_EXECUTE_HANDLER);
}

#endif