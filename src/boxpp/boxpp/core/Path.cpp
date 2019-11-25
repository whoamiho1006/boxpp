#include "Path.hpp"

#if PLATFORM_APPLE
#if !BOX_NOT_COMPILED
#	include <mach-o/dyld.h>
#else
boxpp::s32 _NSGetExecutablePath(char* Buffer, size_t* Size);
#endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

FASTINLINE int __r_mkdir(const boxpp::ansi_t* InPath, int _2) {
	int RetVal = -1;
#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
	RetVal = mkdir(InPath, _2);
#endif
#endif
#if PLATFORM_WINDOWS
	RetVal = mkdir(InPath);
#endif
	return RetVal;
}

FASTINLINE int __r_mkdir(const boxpp::wide_t* InPath, int _2) {
	int RetVal = -1;
#if PLATFORM_POSIX
#if !BOX_NOT_COMPILED
	boxpp::TString<ansi_t> Path = InPath;
	RetVal = __r_mkdir(*Path, _2);
#endif
#endif
#if PLATFORM_WINDOWS
	RetVal = ::_wmkdir(InPath);
#endif

	return RetVal;
}

namespace boxpp {
	FString FPath::DirectoryName(const char_t* InPath)
	{
		s32 Seperator = -1;

		if (InPath) {
			for (s32 i = 0; InPath[i]; i++) {
				if (InPath[i] == '/' || InPath[i] == '\\') {
					Seperator = i;
				}
			}

			if (Seperator >= 0)
				return FString(InPath, Seperator);
		}

		return InPath;
	}

	FString FPath::FileName(const char_t* InPath)
	{
		s32 Seperator = -1;

		if (InPath) {
			for (s32 i = 0; InPath[i]; i++) {
				if (InPath[i] == '/' || InPath[i] == '\\') {
					Seperator = i;
				}
			}

			if (Seperator >= 0)
				return FString(InPath + Seperator + 1);
		}

		return InPath;
	}

	bool FPath::MakeDirectory(const char_t* InPath, bool bRecursive)
	{
		if (!bRecursive) {
			__r_mkdir(InPath, 0777);
		}

		else {
			char_t Temp[512] = { 0, };
			size_t Length = TNativeString<char_t>::Strlen(InPath);

			if (Length) {
				::memcpy(Temp, InPath, sizeof(char_t) * Length);
				
				if (Temp[Length - 1] == '/')
					Temp[Length - 1] = 0;

				for (size_t i = 0; i < Length; i++) {
					if (Temp[i] == '/' || Temp[i] == '\\') {
						char_t Bkp = Temp[i];
						Temp[i] = 0;

						if (!DirectoryExists(Temp)) {
							if (!MakeDirectory(Temp, false))
								return false;
						}

						Temp[i] = Bkp;
					}
				}
			}
		}

		return DirectoryExists(InPath);
	}

	bool FPath::DirectoryExists(const char_t* InPath)
	{
		TString<ansi_t> AnsiPath = InPath;
		struct stat Info;

		if (stat(*AnsiPath, &Info) == 0 &&
			(Info.st_mode & S_IFDIR) == S_IFDIR)
			return true;

		return false;
	}

	bool FPath::FileExists(const char_t* InPath)
	{
		TString<ansi_t> AnsiPath = InPath;
		struct stat Info;

		if (stat(*AnsiPath, &Info) == 0 &&
			(Info.st_mode & S_IFDIR) != S_IFDIR)
			return true;

		return false;
	}

	FString FPath::Binaries()
	{
#if !PLATFORM_APPLE
		using ch_t = char_t;
		static char_t PathName[1024] = { 0, };
#else
		using ch_t = char;
		static char PathName[1024] = { 0, };
#endif

		if (PathName[0] == 0) {
#if PLATFORM_WINDOWS
			w32_compat::GetModuleFileName(nullptr, PathName, (sizeof(PathName) / sizeof(ch_t)));
#endif
#if PLATFORM_APPLE
			size_t Size = sizeof(PathName);
			_NSGetExecutablePath(PathName, &Size);
#endif
#if PLATFORM_POSIX
#endif
			s32 Seperator = -1, Temp = 0;

			while (PathName[Temp] && 
				Temp < (sizeof(PathName) / sizeof(ch_t)))
			{
				if (PathName[Temp] == '/' ||
					PathName[Temp] == '\\')
				{
					Seperator = Temp;
				}

				++Temp;
			}

			if (Seperator >= 0)
				PathName[Seperator] = 0;
		}

		return PathName;
	}

	FString FPath::Configs()
	{
		FString Path = DirectoryName(*Binaries());

		Path.Append("/../configs");

		if (!DirectoryExists(*Path)) {
		}
		

		return *Path;
	}

}