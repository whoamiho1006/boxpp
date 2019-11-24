#pragma once
#include <boxpp.hpp>
#include <boxpp/core/Engine.hpp>
#include <boxpp/module/ModuleManager.hpp>

namespace boxpp
{
	class FBaseEngine : public IEngine
	{
	public:
		FBaseEngine();
		virtual ~FBaseEngine();

	private:
		bool bPreInitialized;
		bool bPreConfigured;
		bool bPostInitialized;
		bool bPostConfigured;

	private:
		bool bKeepRunningLoop;

	protected:
		FBarrior Barrior;
		mutable modules::FModuleManager Modules;
		TArray<TSharedPtr<IEngineLoop>> EngineLoops;

	public:
		/* Initialize engine instance. (INTERNAL USE ONLY) */
		virtual bool Initialize() override;

		/* Finalize engine instance. (INTERNAL USE ONLY) */
		virtual bool Finalize() override;

		/* Determines the finalization is required or not. (INTERNAL USE ONLY) */
		virtual bool ShouldFinalize() const override;

	public:
		/* Get ready state of engine instance. */
		virtual EEngineReady GetReadyState() const override;

	public:
		/* Get module manager. */
		virtual modules::FModuleManager* GetModuleManager() const override;

	public:
		/* Register engine loop. */
		virtual void RegisterLoop(const TSharedPtr<IEngineLoop>& Loop) override;

		/* Unregister engine loop. */
		virtual void UnregisterLoop(const TSharedPtr<IEngineLoop>& Loop) override;

	public:
		/* Keep the loop running or not. */
		virtual bool KeepRunningLoop() const override { return bKeepRunningLoop; }

		/* Terminate engine loop. (after termination, engine will be finalized ) */
		virtual void TerminateLoop() override;

	public:
		/* Pre-Initialize minimal environment. */
		virtual bool PreInitialize() = 0;

		/* Post-Initialize all most features.. */
		virtual bool PostInitialize() = 0;

		/* Pre-Finalize, so, all most features are disabled and disposed. */
		virtual bool PreFinalize() = 0;

		/* Post-Finalize, so, finalized. */
		virtual bool PostFinalize() = 0;

		/* Loads configurations for launching post initialize. */
		virtual bool PreConfigure() = 0;

		/* Loads configurations for launching engine. */
		virtual bool PostConfigure() = 0;

		/* Unloads all configurations. */
		virtual void UnloadConfigurations() = 0;
	};

}