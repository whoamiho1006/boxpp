#pragma once
#include <boxpp.hpp>
#include "BaseEngine.hpp"

namespace boxpp
{
	class FEngine : public FBaseEngine
	{
	public:
		~FEngine();

	public:
		/* Launch engine instance. (INTERNAL USE ONLY) */
		virtual void Launch() override;

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