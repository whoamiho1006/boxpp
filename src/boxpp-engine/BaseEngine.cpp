#include "BaseEngine.hpp"

namespace boxpp {
	FBaseEngine::FBaseEngine()
		: bPreInitialized(false), bPreConfigured(false),
		bPostInitialized(false), bPostConfigured(false),
		bKeepRunningLoop(false)
	{
	}

	FBaseEngine::~FBaseEngine()
	{
	}

	bool FBaseEngine::Initialize() {
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

	bool FBaseEngine::Finalize() {
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

	bool FBaseEngine::ShouldFinalize() const {
		return bPreInitialized || bPreConfigured ||
			bPostInitialized || bPostConfigured;
	}

	EEngineReady FBaseEngine::GetReadyState() const {
		if (bPostConfigured)
			return EEngineReady::Ready;

		else if (bPreConfigured || bPreInitialized)
			return EEngineReady::PreReady;

		return EEngineReady::NotReady;
	}

	modules::FModuleManager* FBaseEngine::GetModuleManager() const {
		if (bPreInitialized)
			return &Modules;

		return nullptr;
	}

	void FBaseEngine::RegisterLoop(const TSharedPtr<IEngineLoop>& Loop) {
		FBarriorScope Guard(Barrior);
		EngineLoops.Add(Loop);
	}

	void FBaseEngine::UnregisterLoop(const TSharedPtr<IEngineLoop>& Loop) {
		FBarriorScope Guard(Barrior);
		EngineLoops.Remove(Loop);
	}

	void FBaseEngine::TerminateLoop() {
		FBarriorScope Guard(Barrior);
		bKeepRunningLoop = false;
	}

}