#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Name.hpp>
#include <boxpp/core/Barrior.hpp>
#include <boxpp/containers/Map.hpp>
#include <boxpp/containers/Queue.hpp>

#include <boxpp/SharedPtr.hpp>
#include <boxpp/core/Engine.hpp>

namespace boxpp
{
#if BOX_COMPILE_BODY
	class FEngine;
	class FBaseEngine;
#endif

	namespace modules
	{
		class IModule;

		class BOXPP FModuleManager
		{
#if BOX_COMPILE_BODY
			friend class FEngine;
			friend class FBaseEngine;
#endif
		public:
			FASTINLINE static FModuleManager* Get() {
				if (IEngine* Engine = IEngine::Get()) {
					return Engine->GetModuleManager();
				}

				return nullptr;
			}

		protected:
			FModuleManager();
			~FModuleManager();

		public:
			/* Get engine module by name. */
			FASTINLINE static IModule* GetModule(const FName& Name) {
				if (FModuleManager* Manager = Get()) {
					return Manager->InternalGetModule(Name);
				}

				return nullptr;
			}

			/* Get weak-ptr of engine module by name. */
			FASTINLINE static TWeakPtr<IModule> GetModuleWeak(const FName& Name) {
				if (FModuleManager* Manager = Get()) {
					return Manager->InternalGetModuleWeak(Name);
				}

				return nullptr;
			}

			/* Get engine module by name. */
			template<typename TModule>
			FASTINLINE static TModule* GetModule(const FName& Name) {
				return dynamic_cast<TModule*>(GetModule(Name));
			}

			/* Get weak-ptr of engine module by name. */
			template<typename TModule>
			FASTINLINE static TWeakPtr<TModule> GetModuleWeak(const FName& Name) {
				return GetModuleWeak(Name);
			}

		private:
			FBarrior Barrior;

			IEngine* Engine;
			bool bStartup;

			TMap<FName, IModule*> Modules;

		protected:
			IModule* InternalGetModule(const FName& Name) const;
			TWeakPtr<IModule> InternalGetModuleWeak(const FName& Name) const;

		protected:
			/* Initialize/Finalize. called by engine. */
			void Initialize(IEngine* Engine);
			void Finalize();

			/* Register/Unregister method is only for engine. */
			bool Register(const FName& Name, IModule* Module);
			bool Unregister(const FName& Name, IModule* Module);

		protected:
			void Startup();
			void Shutdown();
		};
	}
}