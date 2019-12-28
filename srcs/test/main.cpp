#include <boxpp/base/Base.hpp>
#pragma comment(lib, "boxpp.lib")

using namespace boxpp;

int main(int argc, char** argv) {

	FVector3 X = FVector3::Up;

	TSortedArray<int> d;

	d.Add(4);
	d.Add(4);
	d.Add(4);
	X *= 15;

	BOX_ASSERT(false, "hello!");
}