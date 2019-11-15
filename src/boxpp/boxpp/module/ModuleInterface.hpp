#ifndef __BOXPP_MODULE_MODULEINTERFACE_HPP__
#define __BOXPP_MODULE_MODULEINTERFACE_HPP__

#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

namespace boxpp
{
	class IBox;

	namespace modules
	{
#if BOX_COMPILE_BODY
		class FModuleManager;
#endif
		/*
			Module Interface that manages self-initialization.
		*/
		class IModuleInterface
		{

		public:
			virtual ~IModuleInterface() { }

		private:
			IBox* Box;

#if BOX_COMPILE_BODY
		protected:
			friend class FModuleManager;
			class Deployer {
				friend class FModuleManager;

			protected:
				FASTINLINE static void Deploy(
					IModuleInterface* Module, boxpp::IBox* Box)
				{
					Module->Box = Box;
				}
			};
#endif
		public:
			FASTINLINE IBox* GetBox() const { return Box; }

		public:
			virtual bool Startup() = 0;
			virtual void Shutdown() = 0;
		};

	}
}

#endif // !__BOXPP_MODULE_MODULEINTERFACE_HPP__