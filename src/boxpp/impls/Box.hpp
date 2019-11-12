#pragma once
#include <boxpp/IBox.hpp>
#include <boxpp/async/Barrior.hpp>

namespace boxpp
{
	/*
		FBox, IBox implementator.
	*/
	class FBox : public IBox
	{
	public:
		FBox(IBox* Upper);
		~FBox();

	private:
		FBarrior Barrior;
		IBox* Upper;
	};
}