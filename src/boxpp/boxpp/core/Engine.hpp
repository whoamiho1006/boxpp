#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	namespace modules {
		class FModuleManager;
	}

	enum class EEngineReady
	{
		NotReady = 0,
		PreReady,
		Ready
	};

	class IEngine
	{
	public:
		virtual ~IEngine() { }

	public:
		/* Acquire engine instance. */
		BOXPP static IEngine* Get();

	public:
		/* Initialize engine instance. (INTERNAL USE ONLY) */
		virtual bool Initialize() = 0;

		/* Finalize engine instance. (INTERNAL USE ONLY) */
		virtual bool Finalize() = 0;

		/* Launch engine instance. (INTERNAL USE ONLY) */
		virtual void Launch() = 0;

		/* Determines the finalization is required or not. (INTERNAL USE ONLY) */
		virtual bool ShouldFinalize() const = 0;
		
	public:
		/* Get ready state of engine instance. */
		virtual EEngineReady GetReadyState() const = 0;

	public:
		/* Get module manager. */
		virtual modules::FModuleManager* GetModuleManager() const = 0;

	public:
		
	};

}