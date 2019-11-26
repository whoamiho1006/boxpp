#pragma once
#include <boxpp.hpp>

#include <boxpp/module/Module.hpp>
#include <boxpp/module/ModuleManager.hpp>

#include <boxpp/core/ApplicationLoop.hpp>

class FBoxjsLoop : public boxpp::IApplicationLoop
{
public:
	FBoxjsLoop()
		: bKeepLoop(true)
	{
	}

protected:
	/* Init loop */
	virtual void Init() override;

public:
	/* Determines the engine can step or not. */
	virtual bool CanStep() const override { return bKeepLoop; }

	/* Step forward. */
	virtual void Step() override;

	/* Exit engine loop. */
	virtual void Exit() override;

private:
	bool bKeepLoop;
};

class FBoxjsModule : public boxpp::modules::IModule
{
public:
	virtual bool Startup()
	{
		boxpp::IApplication* App = boxpp::IApplication::Get();
		boxpp::FApplicationLoop* Loops = App->GetApplicationLoop();

		Loops->Register(boxpp::MakeShared(new FBoxjsLoop()));
		return true;
	}

	virtual void Shutdown()
	{

	}
};