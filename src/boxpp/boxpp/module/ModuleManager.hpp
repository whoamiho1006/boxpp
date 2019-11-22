#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Name.hpp>
#include <boxpp/core/Barrior.hpp>
#include <boxpp/containers/Map.hpp>

#include <boxpp/SharedPtr.hpp>

namespace boxpp
{
	namespace modules
	{
		class IModule;

		class BOXPP FModuleManager
		{
		public:
			static FModuleManager& Get();

		private:
			FModuleManager();
			~FModuleManager();

		private:
			FBarrior Barrior;
			TMap<FName, IModule*> Modules;

		public:
			IModule* GetModule(const FName& Name) const;
			TWeakPtr<IModule> GetModuleWeak(const FName& Name) const;

		public:
			template<typename TModule>
			TModule* GetModule(const FName& Name) const {
				return dynamic_cast<TModule*>(GetModule(Name));
			}

			template<typename TModule>
			TWeakPtr<TModule> GetModuleWeak(const FName& Name) const {
				return GetModuleWeak(Name);
			}

		public:
			/* INTERNAL USE ONLY! */
			bool Register(const FName& Name, IModule* Module);
			bool Unregister(const FName& Name, IModule* Module);
		};
	}
}