#include "BoxSuper.hpp"

namespace boxpp 
{
	FBoxSuper::FBoxSuper()
	{
	}

	FBoxSuper::~FBoxSuper()
	{
	}

	void FBoxSuper::Register(IBox* ChildBox) {
		BOX_BARRIOR_SCOPED(Barrior);

		if (ChildBox) {
			Children.Add(ChildBox);
		}
	}

	void FBoxSuper::Unregister(IBox* ChildBox) {
		BOX_BARRIOR_SCOPED(Barrior);

		if (ChildBox) {
			ssize_t Index = Children.Find(ChildBox);

			if (Index >= 0) {
				Children.RemoveAt(Index);
			}
		}
	}
}