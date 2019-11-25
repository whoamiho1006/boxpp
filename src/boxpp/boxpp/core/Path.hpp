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
		static FString DirectoryName(const char_t* InPath);
		static FString FileName(const char_t* InPath);

	public:
		static bool MakeDirectory(const char_t* InPath, bool bRecursive = true);

	public:
		static bool DirectoryExists(const char_t* InPath);
		static bool FileExists(const char_t* InPath);

	public:
		static FString Binaries();
		static FString Configs();

	public:

	};
}