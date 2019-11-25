#include "BaseApplication.hpp"

namespace boxpp {
	FBaseApplication::FBaseApplication()
		: bPreInitialized(false), bPreConfigured(false),
		bPostInitialized(false), bPostConfigured(false),
		bKeepRunningLoop(false)
	{
		Loop.SetEngine(this);
	}

	FBaseApplication::~FBaseApplication()
	{
	}

	bool FBaseApplication::Initialize() {
		if (!bPreInitialized) {
			bPreInitialized = PreInitialize();
		}

		if (bPreInitialized) {
			Modules.Initialize(this);
			bPreConfigured = PreConfigure();
		}

		if (bPreConfigured) {
			bPostInitialized = PostInitialize();
		}

		if (bPostInitialized) {
			bPostConfigured = PostConfigure();
		}

		if (bPostConfigured) {
			Modules.Startup();
		}

		return bPostConfigured;
	}

	bool FBaseApplication::Finalize() {
		if (bPostConfigured) {
			Modules.Shutdown();
		}

		if (bPostInitialized) {
			bPostInitialized = !PreFinalize();
		}

		if (bPreInitialized) {
			Modules.Finalize();
			bPreInitialized = !PostFinalize();
		}

		if (bPostConfigured || bPreConfigured) {
			UnloadConfigurations();
		}

		return !bPreInitialized;
	}

	bool FBaseApplication::ShouldFinalize() const {
		return bPreInitialized || bPreConfigured ||
			bPostInitialized || bPostConfigured;
	}

	EEngineReady FBaseApplication::GetReadyState() const {
		if (bPostConfigured)
			return EEngineReady::Ready;

		else if (bPreConfigured || bPreInitialized)
			return EEngineReady::PreReady;

		return EEngineReady::NotReady;
	}

	modules::FModuleManager* FBaseApplication::GetModuleManager() const {
		if (bPreInitialized)
			return &Modules;

		return nullptr;
	}

	FApplicationLoop* FBaseApplication::GetEngineLoop() const {
		if (bPostInitialized)
			return &Loop;

		return nullptr;
	}

}