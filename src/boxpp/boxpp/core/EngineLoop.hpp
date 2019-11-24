#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	class IEngine;
	class IEngineLoop
	{
	public:
		virtual ~IEngineLoop() { }

	private:
		IEngine* Engine;

	public:
		/* Determines the engine can step or not. */
		virtual bool CanStep() const = 0;
		
		/* Step forward. */
		virtual void Step() = 0;

		/* Exit engine loop. */
		virtual void Exit() = 0;
	};

}
