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

	EReadyState FBaseApplication::GetReadyState() const {
		if (bPostConfigured)
			return EReadyState::Ready;

		else if (bPreConfigured || bPreInitialized)
			return EReadyState::PreReady;

		return EReadyState::NotReady;
	}

	modules::FModuleManager* FBaseApplication::GetModuleManager() const {
		if (bPreInitialized)
			return &Modules;

		return nullptr;
	}

	FApplicationLoop* FBaseApplication::GetApplicationLoop() const {
		if (bPostInitialized)
			return &Loop;

		return nullptr;
	}

}