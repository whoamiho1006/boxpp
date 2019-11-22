#include "ModuleManager.hpp"

#include <boxpp/module/Module.hpp>

namespace boxpp
{
	namespace modules
	{
		FModuleManager& FModuleManager::Get()
		{
			static FModuleManager ModuleManager;
			return ModuleManager;
		}

		FModuleManager::FModuleManager()
		{
		}

		FModuleManager::~FModuleManager()
		{
		}

		IModule* FModuleManager::GetModule(const FName& Name) const
		{
			FBarriorScope Guard(Barrior);
			return Modules.ContainsKey(Name) ? Modules[Name] : nullptr;
		}

		TWeakPtr<IModule> FModuleManager::GetModuleWeak(const FName& Name) const
		{
			FBarriorScope Guard(Barrior);

			if (Modules.ContainsKey(Name))
				return Modules[Name]->ToWeak();

			return nullptr;
		}

		bool FModuleManager::Register(const FName& Name, IModule* Module)
		{
			FBarriorScope Guard(Barrior);

			if (!Modules.ContainsKey(Name)) {
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
				return true;
			}

			return false;
		}
	}
}