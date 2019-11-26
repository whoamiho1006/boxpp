#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/String.hpp>

namespace boxpp
{
	namespace modules {
		class FModuleManager;
	}

	enum class EReadyState
	{
		NotReady = 0,
		PreReady,
		Ready
	};

	class IApplicationLoop;
	class FApplicationLoop;

	class IApplication
	{
	public:
		virtual ~IApplication() { }

	public:
		/* Acquire engine instance. */
		BOXPP static IApplication* Get();

	public:
		/* Initialize engine instance. (INTERNAL USE ONLY) */
		virtual bool Initialize() = 0;

		/* Finalize engine instance. (INTERNAL USE ONLY) */
		virtual bool Finalize() = 0;

		/* Determines the finalization is required or not. (INTERNAL USE ONLY) */
		virtual bool ShouldFinalize() const = 0;

		/* Get command-line arguments. */
		virtual const TArray<FString>& GetArguments() const = 0;
		
	public:
		/* Get ready state of engine instance. */
		virtual EReadyState GetReadyState() const = 0;

	public:
		/* Get module manager. */
		virtual modules::FModuleManager* GetModuleManager() const = 0;

		/* Get engine loop. */
		virtual FApplicationLoop* GetApplicationLoop() const = 0;
	};

}