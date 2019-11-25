#include "ModuleManager.hpp"

#include <boxpp/module/Module.hpp>

namespace boxpp
{
	namespace modules
	{
		FModuleManager::FModuleManager()
			: Engine(nullptr)
		{
		}

		FModuleManager::~FModuleManager()
		{
		}

		IModule* FModuleManager::InternalGetModule(const FName& Name) const
		{
			FBarriorScope Guard(Barrior);
			return Modules.ContainsKey(Name) ? Modules[Name] : nullptr;
		}

		TWeakPtr<IModule> FModuleManager::InternalGetModuleWeak(const FName& Name) const
		{
			FBarriorScope Guard(Barrior);

			if (Modules.ContainsKey(Name))
				return Modules[Name]->ToWeak();

			return nullptr;
		}

		void FModuleManager::Initialize(IApplication* Engine)
		{
			this->Engine = Engine;
		}

		void FModuleManager::Finalize()
		{
			this->Engine = nullptr;
		}

		bool FModuleManager::Register(const FName& Name, IModule* Module)
		{
			FBarriorScope Guard(Barrior);

			if (!Modules.ContainsKey(Name)) {
				if (bStartup && !Module->Startup()) {
					return false;
				}

				Modules.Emplace(Name, Module);
				return true;
			}

			return false;
		}

		bool FModuleManager::Unregister(const FName& Name, IModule* Module)
		{
			FBarriorScope Guard(Barrior);

			if (Modules.ContainsKey(Name) &&
				Modules[Name] == Module) 
			{
				Modules.Remove(Name);

				if (bStartup) {
					Module->Shutdown();
				}
				return true;
			}

			return false;
		}
		
		void FModuleManager::Startup()
		{
			FBarriorScope Guard(Barrior);
			TArray<FName> Names;
			TQueue<FName> FailedNames;

			bStartup = true;
			Modules.GetKeys(Names);

			for (const FName& Name : Names) {
				if (!Modules[Name]->Startup()) {
					FailedNames.Enqueue(Name);
				}
			}

			while (!FailedNames.IsEmpty()) {
				Modules.Remove(*FailedNames.Peek());
				FailedNames.Dequeue();
			}
		}
		
		void FModuleManager::Shutdown()
		{
			FBarriorScope Guard(Barrior);
			TArray<FName> Names;

			bStartup = false;
			Modules.GetKeys(Names);

			for (const FName& Name : Names) {
				Modules[Name]->Shutdown();
			}
		}
	}
}