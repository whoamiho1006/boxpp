#include <boxpp/base/Base.hpp>
#pragma comment(lib, "boxpp.lib")

using namespace boxpp;

int main(int argc, char** argv) {

	FVector3 X = FVector3::Up;

	X *= 15;

	BOX_ASSERT(false, "hello!");
}