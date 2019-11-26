#include <boxpp.hpp>

#include <boxpp/module/Module.hpp>
#include <boxpp/module/ModuleManager.hpp>
#include <boxpp/core/Path.hpp>

#include <boxpp/containers/Delegate.hpp>
#include <boxpp/containers/Method.hpp>

#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")


class Test
{
public:
	int good() const {
		printf("hello2\n");
		return 0;
	}
};

void h() {
	boxpp::TMethod<int()> Test2 = &Test::good;

	printf("%d\n", Test2(new Test()));
}

class FAppModule : public boxpp::modules::IModule
{
public:
	virtual bool Startup()
	{
		printf("Hello! %S\n", *boxpp::FPath::Binaries());
		h();
		return true;
	}

	virtual void Shutdown()
	{

	}
};

BOX_IMPLEMENT_MODULE(FAppModule, AppModule);