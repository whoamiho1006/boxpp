#include "Engine.hpp"

namespace boxpp {

	FEngine::~FEngine()
	{
	}

	void FEngine::Launch()
	{
	}

	bool FEngine::PreInitialize()
	{
		return false;
	}

	bool FEngine::PostInitialize()
	{
		return false;
	}

	bool FEngine::PreFinalize()
	{
		return false;
	}

	bool FEngine::PostFinalize()
	{
		return false;
	}

	bool FEngine::PreConfigure()
	{
		return false;
	}

	bool FEngine::PostConfigure()
	{
		return false;
	}

	void FEngine::UnloadConfigurations()
	{
	}

}