#include <boxpp.hpp>
#include <boxpp/module/Module.hpp>

#include <boxpp/containers/Delegate.hpp>

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

class FAppModule : public boxpp::modules::IModule
{
public:
	virtual bool Startup()
	{
		boxpp::TDelegate<int, short> dd;

		return true;
	}

	virtual void Shutdown()
	{

	}
};