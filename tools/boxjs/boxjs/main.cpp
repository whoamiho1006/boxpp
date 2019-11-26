#include "Context.hpp"
#include <stdio.h>


int main(int argc, char** argv) {
	FContext& Context = FContext::Get();

	if (argc < 2) {
		fprintf(stderr, "fatal: arguments lack.\n");
		fprintf(stderr, "usage: boxjs [SCRIPT] ...\n");
		return EXIT_FAILURE;
	}

	Context.Evaluate(argv[1]);
	if (Context.IsFatalError())
		return EXIT_FAILURE;

	return Context.Execute(argc, argv);
}