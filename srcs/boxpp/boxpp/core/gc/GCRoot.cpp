#include "GCRoot.hpp"
#include "GCObject.hpp"

namespace boxpp
{
	namespace core
	{
		namespace gc
		{
			FGCRoot& FGCRoot::Get()
			{
				static FGCRoot Root;
				return Root;
			}

			FGCRoot::FGCRoot()
			{
			}

			FGCRoot::~FGCRoot()
			{
				TArray<IGCObject*> GCObjects;
				{
					FBarriorScope Guard(Barrior);

					for (IGCObject* Object : Objects) {
						GCObjects.Add(Object);
					}

					Objects.Clear();
				}

				for (IGCObject* Object : GCObjects)
				{
					if (Object->IsValid())
						Object->Destroy();
				}

				for (IGCObject* Object : GCObjects)
				{
					FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(Object);
					if (Entry->HasFlags(EGCF_KillPending))
					{
						delete Object;
					}
				}
			}

			void FGCRoot::Register(IGCObject* Object)
			{
				FBarriorScope Guard(Barrior);
				FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(Object);
				BOX_ASSERT(!Entry->HasAnyFlags(EGCF_Destroying || EGCF_KillPending),
					"FATAL! Tried to register already destroyed(or destroying) GC-object.");

				Objects.Add(Object);

				/* If GC Worker isn't alive, run it. */
				if (!GCWorker::Get().IsAlive()) {
					GCWorker::Get().Run();
				}
			}

			void FGCRoot::Unregister(IGCObject* Object)
			{
				FBarriorScope Guard(Barrior);
				FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(Object);
				BOX_ASSERT(Entry->HasFlags(EGCF_Destroying || EGCF_KillPending),
					"FATAL! Tried to unregister alive GC-object.");

				Objects.Remove(Object);
			}

			FGCRoot::GCWorker& FGCRoot::GCWorker::Get()
			{
				static GCWorker* Worker = nullptr;
				static FBarrior Barrior;

				FBarriorScope Guard(Barrior);

				while (true) {
					if (!Worker) {
						Worker = new GCWorker();
					}

					else if (!Worker->IsAlive()) {
						if (Worker->HasRan()) {
							delete Worker;
							Worker = nullptr;
							continue;
						}
					}

					break;
				}

				return *Worker;
			}

			void FGCRoot::GCWorker::OnRun()
			{
				FGCRoot& Root = FGCRoot::Get();
				TArray<IGCObject*> GCObjects;

				while (true) {
					{
						FBarriorScope Guard(Root.Barrior);
						for (IGCObject* Object : Root.Objects) {
							GCObjects.Add(Object);
						}
					}

					GCObjects.Optimize();
					if (GCObjects.GetSize() <= 0)
						break;

					for (IGCObject* Object : GCObjects)
					{
						FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(Object);

						if (Object->IsValid() && !Entry->GetUpper() &&
							!Entry->HasFlags(EGCF_RootSet))
						{
							// Destroy the object.
							Object->Destroy();
						}

						else if (Entry->HasFlags(EGCF_KillPending)) {
							delete Object;
						}
					}

					GCObjects.Clear(true);
				}
			}
		}
	}
}