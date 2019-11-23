#include "boilerplate.hpp"
#include <boxpp/core/Engine.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;
using namespace boxpp::boilerplates;

extern int commonMain();

int main(int argc, char** argv)
{
	FBoilerplate::Set(FBoxBoilerplate::Get());
	FLogging::Get().SetUnderCLI(true);
	int RetVal = commonMain();
	FBoilerplate::Set(nullptr);
	return RetVal;
}


