#include <boxpp/base/Base.hpp>
#include <boxpp/base/BaseNetwork.hpp>
#pragma comment(lib, "boxpp.lib")

BOXPP_DECLARE_MEMORY_BOILERPLATE()
using namespace boxpp;

int main(int argc, char** argv) {
	FTcpListener Server(EProtocolType::Inet, 8000);
	TLinkedList<int> dd;
	BOX_ASSERT(Server.Start(), "Start() failed");

	int cnt = 0;

	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.Add(100);
	dd.RemoveAll(100);

	while (cnt < 10) {
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

		++cnt;
	}

	//BOX_ASSERT(false, "hello!");
}