#include <boxpp/core/Application.hpp>
#include <boxpp/core/Logging.hpp>

using namespace boxpp;

extern int commonMain(int argc, char** argv);

int main(int argc, char** argv)
{
	FLogging::Get().SetUnderCLI(true);
	int RetVal = commonMain(argc, argv);
	return RetVal;
}


