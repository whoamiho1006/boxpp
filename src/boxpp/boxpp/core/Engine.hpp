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

	class IEngineLoop;
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
		/* Register engine loop. */
		virtual void RegisterLoop(const TSharedPtr<IEngineLoop>& Loop) = 0;

		/* Unregister engine loop. */
		virtual void UnregisterLoop(const TSharedPtr<IEngineLoop>& Loop) = 0;

	public:
		/* Keep the loop running or not. */
		virtual bool KeepRunningLoop() const = 0;

		/* Terminate engine loop. (after termination, engine will be finalized ) */
		virtual void TerminateLoop() = 0;
	};

}