#include "Application.hpp"
#include <boxpp/core/Threading.hpp>
#include <boxpp/core/ApplicationLoop.hpp>

namespace boxpp {

	FApplication::~FApplication()
	{
	}

	bool FApplication::PreInitialize()
	{
		return true;
	}

	bool FApplication::PostInitialize()
	{
		return true;
	}

	bool FApplication::PreFinalize()
	{
		return true;
	}

	bool FApplication::PostFinalize()
	{
		return true;
	}

	bool FApplication::PreConfigure()
	{
		return true;
	}

	bool FApplication::PostConfigure()
	{
		return true;
	}

	void FApplication::UnloadConfigurations()
	{
	}

}