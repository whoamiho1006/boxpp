#pragma once
#include <boxpp/IBox.hpp>
#include <boxpp/async/Barrior.hpp>
#include <boxpp/containers/SortedArray.hpp>

namespace boxpp
{
	/*
		FBoxSuper.
		Top layer of IBox instances.
	*/
	class FBoxSuper : public IBox
	{
	public:
		FBoxSuper();
		~FBoxSuper();

	private:
		FBarrior Barrior;
		TSortedArray<TWeakPtr<IBox, ESharedMode::Safe>> Children;

	public:
		/* Registers the child box. */
		void Register(const TWeakPtr<IBox, ESharedMode::Safe>& ChildBox);

		/* Unregisters the child box.*/
		void Unregister(const TWeakPtr<IBox, ESharedMode::Safe>& ChildBox);

	};
}