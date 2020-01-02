#include <boxpp/base/Base.hpp>
#include <boxpp/base/BaseNetwork.hpp>
#include <boxpp/cli/TTY.hpp>
#include <boxpp/cli/CommandLine.hpp>
#include <boxpp/cli/Arguments.hpp>

#pragma comment(lib, "boxpp.lib")

BOXPP_DECLARE_MEMORY_BOILERPLATE()
using namespace boxpp;
using namespace boxpp::cli;

int main(int argc, char** argv) {
	FTcpListener Server(EProtocolType::Inet, 8000);
	TLinkedList<int> dd;
	BOX_ASSERT(Server.Start(), "Start() failed");

	FCommandLine mm;
	FArgumentSet arg("n", "name");

	bool b = TTypedArgument<int>::Supported;
	s32 k = 0;

	arg.Requires(k, "name", "specifies name of !!...");

	FString TestArg;
	s32 Greeting;

	FTTY test;

	mm.PutArgs(argv, argc);
	mm.PutLine("--test good hello");

	mm.On("t", "test", "test switch")
		.Requires(TestArg, "name", "name of test arg")
		.Requires(Greeting, "greeting", "greeting text")
		.SetCallback([&](FArgumentSet* p) {
			test << TestArg << EOL;
			test << Greeting << EOL;

			p->SetUserError();
		});

	mm.Invoke();

	test << 33 << EOL;
	test << 34.5 << EOL;

	FTTY test2 = FTTY::Error;

	test << FTTYColor::Red << "hello!" << EOL;
	//test2 << Goto(0, 0) << "test" << EOL;

	FString test3;
	
	test << "Password: ";
	test << NEC >> test3 << ECH;

	test << *test3 << EOL;
	//BOX_ASSERT(false, "hello!");
}