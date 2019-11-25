#include <boxpp.hpp>

#include <boxpp/module/Module.hpp>
#include <boxpp/module/ModuleManager.hpp>

#include <boxpp/containers/Delegate.hpp>

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

class FAppModule : public boxpp::modules::IModule
{
public:
	virtual bool Startup()
	{
		printf("Hello!");
		return true;
	}

	virtual void Shutdown()
	{

	}
};

BOX_IMPLEMENT_MODULE(FAppModule, AppModule);