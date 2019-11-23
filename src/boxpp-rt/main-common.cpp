#include "boilerplate.hpp"
#include <boxpp/core/Engine.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;

int commonMain()
{
	if (IEngine* Engine = IEngine::Get()) {
		if (Engine->Initialize()) {

			Engine->Launch();

			if (!Engine->Finalize()) {
				BX_LOG(Fatal, LogCore, BOXTEXT("Engine couldn't be finalized!"));
				return EXIT_FAILURE;
			}
		}

		else 
		{
			if (Engine->ShouldFinalize() && !Engine->Finalize())
				BX_LOG(Fatal, LogCore, BOXTEXT("Engine couldn't be finalized!"));
			
			else BX_LOG(Fatal, LogCore, BOXTEXT("Engine couldn't be initialized!"));
			return EXIT_FAILURE;
		}
	}

	else {
		BX_LOG(Fatal, LogCore, BOXTEXT("Engine couldn't be created!"));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


