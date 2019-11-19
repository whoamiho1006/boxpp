#include "BoxSuper.hpp"

namespace boxpp 
{
	FBoxSuper::FBoxSuper()
	{
	}

	FBoxSuper::~FBoxSuper()
	{
	}

	void FBoxSuper::Register(const TWeakPtr<IBox, ESharedMode::Safe>& ChildBox) {
		BOX_BARRIOR_SCOPED(Barrior);

		if (ChildBox) {
			Children.Add(ChildBox);
		}
	}

	void FBoxSuper::Unregister(const TWeakPtr<IBox, ESharedMode::Safe>& ChildBox) {
		BOX_BARRIOR_SCOPED(Barrior);

		if (ChildBox) {
			ssize_t Index = Children.IndexOf(ChildBox);

			if (Index >= 0) {
				Children.RemoveAt(Index);
			}
		}
	}
}