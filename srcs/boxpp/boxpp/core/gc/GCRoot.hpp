#pragma once
#include <boxpp/base/Base.hpp>

namespace boxpp
{
	namespace core
	{
		namespace gc
		{
			class IGCObject;

			/*
				GCRoot.
				Garbage Collector's Root.
			*/
			class BOXPP FGCRoot
			{
			public:
				static FGCRoot& Get();

			private:
				class GCWorker : public IThreaded
				{
				public:
					static GCWorker& Get();

				protected:
					virtual void OnRun() override;
				};

			private:
				FGCRoot();
				~FGCRoot();

			private:
				FBarrior Barrior;
				TSortedArray<IGCObject*> Objects;

			public:
				/* Register a GC object. */
				void Register(IGCObject* Object);

				/* Unregister a GC object. */
				void Unregister(IGCObject* Object);

				FBarrior& GetBarrior() { return Barrior; }
			};
		}
	}
}