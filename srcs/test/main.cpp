#include <boxpp/base/Base.hpp>
#include <boxpp/base/BaseNetwork.hpp>
#include <boxpp/cli/TTY.hpp>

#pragma comment(lib, "boxpp.lib")

BOXPP_DECLARE_MEMORY_BOILERPLATE()
using namespace boxpp;
using namespace boxpp::cli;

int main(int argc, char** argv) {
	FTcpListener Server(EProtocolType::Inet, 8000);
	TLinkedList<int> dd;
	BOX_ASSERT(Server.Start(), "Start() failed");

	FTTY test;
	FTTY test2 = FTTY::Error;

	test << FTTYColor::Red << "hello!" << EOL;
	test2 << Goto(0, 0) << "test" << EOL;

	FString test3;
	
	test << "Password: ";
	test << NEC >> test3 << ECH;

	test << *test3 << EOL;
	//BOX_ASSERT(false, "hello!");
}