#pragma once
#include <boxpp.hpp>
#include <boxpp/core/Application.hpp>
#include <boxpp/core/ApplicationLoop.hpp>
#include <boxpp/module/ModuleManager.hpp>

namespace boxpp
{
	class FBaseApplication : public IApplication
	{
	public:
		FBaseApplication();
		virtual ~FBaseApplication();

	private:
		bool bPreInitialized;
		bool bPreConfigured;
		bool bPostInitialized;
		bool bPostConfigured;

	private:
		bool bKeepRunningLoop;

	protected:
		FBarrior Barrior;
		mutable FApplicationLoop Loop;
		mutable modules::FModuleManager Modules;
		TArray<TSharedPtr<IApplicationLoop>> EngineLoops;
		TArray<FAnsiString> Arguments;

	public:
		/* Initialize engine instance. (INTERNAL USE ONLY) */
		virtual bool Initialize() override;

		/* Finalize engine instance. (INTERNAL USE ONLY) */
		virtual bool Finalize() override;

		/* Determines the finalization is required or not. (INTERNAL USE ONLY) */
		virtual bool ShouldFinalize() const override;

		/* Get command-line arguments. */
		virtual const TArray<FAnsiString>& GetArguments() const override {
			return Arguments;
		}

	public:
		/* Get ready state of engine instance. */
		virtual EReadyState GetReadyState() const override;

	public:
		/* Get module manager. */
		virtual modules::FModuleManager* GetModuleManager() const override;

		/* Get engine loop. */
		virtual FApplicationLoop* GetApplicationLoop() const override;

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