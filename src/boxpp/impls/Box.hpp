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
		FBox(const TWeakPtr<IBox, ESharedMode::Safe>& Upper);
		~FBox();

	private:
		FBarrior Barrior;
		TWeakPtr<IBox, ESharedMode::Safe> Upper;

	};
}