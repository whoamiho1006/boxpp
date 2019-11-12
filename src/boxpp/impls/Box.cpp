#include "Box.hpp"

namespace boxpp 
{
	FBox::FBox(IBox* Upper)
		: Upper(Upper)
	{
	}

	FBox::~FBox()
	{
	}
}