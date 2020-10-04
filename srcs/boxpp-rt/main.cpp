#include <boxpp/base/Base.hpp>
#include <boxpp/cli/CommandLine.hpp>
#include <boxpp/cli/TTY.hpp>

using namespace boxpp;
using namespace boxpp::cli;

BOXPP_DECLARE_MEMORY_BOILERPLATE()

class FBoxRT {
public:
	FBoxRT() {
	}

private:
	FCommandLine Cmd;

public:
	int Main(int Argc, char** Argv) {
		FTTY Outs(FTTY::Output), Errs(FTTY::Error);
		s32 RetVal = EXIT_SUCCESS;


		return RetVal;
	}
};

int main(int argc, char** argv) {
	return FBoxRT().Main(argc, argv);
}

