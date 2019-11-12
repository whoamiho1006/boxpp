#pragma once
#include <boxpp/IBox.hpp>
#include <boxpp/async/Barrior.hpp>
#include <impls/utils/PureArray.hpp>

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
		TPureArray<IBox*> Children;

	public:
		/* Registers the child box. */
		void Register(IBox* ChildBox);

		/* Unregisters the child box.*/
		void Unregister(IBox* ChildBox);
	};
}