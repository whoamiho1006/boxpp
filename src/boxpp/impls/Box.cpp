#include "Box.hpp"

namespace boxpp 
{
	FBox::FBox(const TWeakPtr<IBox, ESharedMode::Safe>& Upper)
		: Upper(Upper), Allocator(Upper)
	{
	}

	FBox::~FBox()
	{
	}
}