#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Name.hpp>
#include <boxpp/traits/Forward.hpp>
#include <boxpp/containers/Array.hpp>

namespace boxpp
{
	class BOXPP FPath
	{
	public:
		static FString DirectoryName(const wide_t* InPath);
		static FString FileName(const wide_t* InPath);

		static FAnsiString DirectoryName(const ansi_t* InPath);
		static FAnsiString FileName(const ansi_t* InPath);

	public:
		static bool MakeDirectory(const ansi_t* InPath, bool bRecursive = true);
		FASTINLINE static bool MakeDirectory(const wide_t* InPath, bool bRecursive = true) {
			FAnsiString Temp(InPath);
			return MakeDirectory(Temp.GetRaw(), bRecursive);
		}

	public:
		static bool DirectoryExists(const ansi_t* InPath);
		static bool FileExists(const ansi_t* InPath);

		FASTINLINE static bool DirectoryExists(const wide_t* InPath) {
			FAnsiString Temp(InPath);
			return DirectoryExists(Temp.GetRaw());
		}

		FASTINLINE static bool FileExists(const wide_t* InPath) {
			FAnsiString Temp(InPath);
			return FileExists(Temp.GetRaw());
		}

	public:
		static void GetCwd(FString& OutPath);
		static void GetCwd(FAnsiString& OutPath);
		static void SetCwd(const FAnsiString& Cwd);
		FASTINLINE static void SetCwd(const FString& Cwd) { SetCwd((FAnsiString) Cwd); }

		static void Binaries(FString& OutPath);
		static void Binaries(FAnsiString& OutPath);

		FASTINLINE static void Configs(FString& OutPath) {
			FString BinaryPath;
			OutPath.Clear();

			Binaries(BinaryPath);
			OutPath.Append(DirectoryName(*BinaryPath));
			OutPath.Append(BOXTEXT("/../configs"));

			if (!DirectoryExists(*OutPath)) {
				MakeDirectory(*OutPath);
			}
		}

		FASTINLINE static void Configs(FAnsiString& OutPath) {
			FAnsiString BinaryPath;
			OutPath.Clear();

			Binaries(BinaryPath);
			OutPath.Append(DirectoryName(*BinaryPath));
			OutPath.Append("/../configs");

			if (!DirectoryExists(*OutPath)) {
				MakeDirectory(*OutPath);
			}
		}

	public:

	};
}