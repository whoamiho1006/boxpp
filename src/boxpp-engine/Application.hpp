#pragma once
#include <boxpp.hpp>
#include "BaseApplication.hpp"

namespace boxpp
{
	class FApplication : public FBaseApplication
	{
	public:
		~FApplication();

	public:
		/* Pre-Initialize minimal environment. */
		virtual bool PreInitialize() override;

		/* Post-Initialize all most features.. */
		virtual bool PostInitialize() override;

		/* Pre-Finalize, so, all most features are disabled and disposed. */
		virtual bool PreFinalize() override;

		/* Post-Finalize, so, finalized. */
		virtual bool PostFinalize() override;

		/* Loads configurations for launching post initialize. */
		virtual bool PreConfigure() override;

		/* Loads configurations for launching engine. */
		virtual bool PostConfigure() override;

		/* Unloads all configurations. */
		virtual void UnloadConfigurations() override;
	};

}