#include <boxpp/base/Base.hpp>
#include <boxpp/base/BaseNetwork.hpp>
#pragma comment(lib, "boxpp.lib")

using namespace boxpp;

int main(int argc, char** argv) {
	FTcpListener Server(EProtocolType::Inet, 8000);
	BOX_ASSERT(Server.Start(), "Start() failed");

	while (true) {
		FTcpClient Newbie;

		if (!Server.IsPending()) {
			
			continue;
		}

		if (Server.Accept(Newbie)) {
			FIPEndpoint Endpoint;
			const char* Packet = "HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html; charset=utf-8\r\n"
				"Content-Length: 20\r\n\r\n"
				"abcdabcdabcdabcdabcd\r\n";

			if (Newbie.GetSocket()->GetSockName(Endpoint)) {
				printf("%S\n", Endpoint.ToString().GetRaw());
			}

			Newbie.Send(Packet, TNativeString<char>::Strlen(Packet));
			Newbie.Disconnect();
		}
	}

	BOX_ASSERT(false, "hello!");
}