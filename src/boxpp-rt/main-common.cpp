#include <boxpp/Boilerplate.hpp>
#include <boxpp/core/Application.hpp>
#include <boxpp/core/ApplicationLoop.hpp>
#include <boxpp/core/Logging.hpp>

#pragma comment(lib, "boxpp.lib")
BOXPP_DECLARE_BOILERPLATE();

using namespace boxpp;
using namespace boxpp::boilerplates;

NO_MANGLED BOXEXPORT void BOX_ModuleMain(IApplication* App);

int commonMain()
{
	if (IApplication* Engine = IApplication::Get()) {
		if (Engine->Initialize()) {
			/* Execute module-main of this executable. */
			BOX_ModuleMain(Engine);

			/* and then, launch loop. */
			if (FApplicationLoop* Loop = Engine->GetEngineLoop()) {
				while (Loop->CanStep()) {
					Loop->Step();
				}

				Loop->Exit();
			}

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


