#pragma once
#include <boxpp/IBox.hpp>
#include <boxpp/async/Barrior.hpp>
#include <impls/memory/Allocator.hpp>

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
		FAllocator Allocator;
		TWeakPtr<IBox, ESharedMode::Safe> Upper;

	public:
		/* Memory allocator for this box. */
		virtual IAllocator* GetAllocator() override { return &Allocator; }
	};
}