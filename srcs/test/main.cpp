#include <boxpp/base/Base.hpp>
#pragma comment(lib, "boxpp.lib")

using namespace boxpp;

int main(int argc, char** argv) {
	FSocket Test(EProtocolType::Inet, ESocketType::Tcp);

	BOX_ASSERT(Test.Bind(FIPAddress::Any, 8000), "Bind() failed");
	BOX_ASSERT(Test.Listen(30), "Listen() failed");

	while (!Test.HasError()) {
		FSocket Newbie;

		if (Test.Accept(Newbie)) {
			const char* Packet = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html; charset=utf-8\r\n"
				"Content-Length: 20\r\n\r\n"
				"abcdabcdabcdabcdabcd\r\n";

			Newbie.Send(Packet, TNativeString<char>::Strlen(Packet));
			Newbie.Shutdown();
		}
	}

	BOX_ASSERT(false, "hello!");
}