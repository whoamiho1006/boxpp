#include "Application.hpp"
#include <Application.hpp>

namespace boxpp {

	IApplication* IApplication::Get()
	{
		static FApplication App;
		return &App;
	}

}