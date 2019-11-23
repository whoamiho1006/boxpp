#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/sharedptrs/BroadcastedObject.hpp>

namespace boxpp
{
	class IEngine;

	namespace modules
	{
		/*
			Module Interface that manages self-initialization.
		*/
		class IModule 
			: public IBroadcastedObject<IModule, ESharedMode::Safe>
		{
		public:
			virtual ~IModule() { }

		public:
			virtual bool Startup() = 0;
			virtual void Shutdown() = 0;
		};

	}
}
