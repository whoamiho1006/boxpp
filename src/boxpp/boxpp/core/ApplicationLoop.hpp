#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Barrior.hpp>
#include <boxpp/SharedPtr.hpp>

#include <boxpp/containers/Array.hpp>

namespace boxpp
{
	class IApplication;
	class FApplicationLoop;

	class IApplicationLoop
	{
		friend class FApplicationLoop;

	public:
		virtual ~IApplicationLoop() { }

	private:
		IApplication* Engine;

	protected:
		class Deployer {
			friend class FApplicationLoop;

		protected:
			FASTINLINE static void Deploy(IApplicationLoop* Loop, IApplication* Engine) {
				Loop->Engine = Engine;
			}
		};

	public:
		/* Determines the engine can step or not. */
		virtual bool CanStep() const = 0;

		/* Step forward. */
		virtual void Step() = 0;

		/* Exit engine loop. */
		virtual void Exit() = 0;

	protected:
		/* Init loop */
		virtual void Init() = 0;
	};

	class BOXPP FApplicationLoop : public IApplicationLoop
	{
	public: 
		FApplicationLoop() 
			: bKeepRunning(true), bStepping(false), bLoopKilled(false)
		{
		}
		~FApplicationLoop() { }

	private:
		FBarrior Barrior;
		TArray<TSharedPtr<IApplicationLoop>> Subloops;
		TArray<TSharedPtr<IApplicationLoop>> CurrentLoops;
		bool bKeepRunning, bStepping, bLoopKilled;

	public:
		void SetEngine(IApplication* Engine) {
			this->Engine = Engine;
		}

	public:
		bool Register(const TSharedPtr<IApplicationLoop>& Loop);
		bool Unregister(const TSharedPtr<IApplicationLoop>& Loop);

	private:
		void PrepareCurrent();

	public:
		/* Determines the engine can step or not. */
		virtual bool CanStep() const override;

		/* Step forward. */
		virtual void Step() override;

		/* Exit engine loop. */
		virtual void Exit() override;

	protected:
		/* Init loop */
		virtual void Init() override;

	};
}
