#include "Engine.hpp"
#include <Engine.hpp>

namespace boxpp {

	IEngine* IEngine::Get()
	{
		static FEngine Engine;
		return &Engine;
	}

}