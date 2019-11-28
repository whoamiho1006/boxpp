#include "GCObject.hpp"
#include "GCRoot.hpp"

namespace boxpp
{
	namespace core
	{
		namespace gc
		{
			FRawTls& IGCObject::Tls()
			{
				static FRawTls _Tls;
				return _Tls;
			}

			IGCObject::IGCObject()
			{
				IGCObject* TlsUpper = (IGCObject*)Tls().Get();
				GCEntry.SetSelf(this);

				if (TlsUpper) {
					GCEntry.SetUpper(TlsUpper);
				}
			}

			IGCObject::IGCObject(IGCObject* Upper)
			{
				IGCObject* TlsUpper = (IGCObject*)Tls().Get();
				GCEntry.SetSelf(this);

				if (Upper) {
					GCEntry.SetUpper(Upper);
				}
				else if (TlsUpper) {
					GCEntry.SetUpper(TlsUpper);
				}
			}

			IGCObject::~IGCObject()
			{
				FGCRoot::Get().Unregister(this);
				GCEntry.SetSelf(nullptr);
			}

			void IGCObject::INTERNALS::Initialize(IGCObject* Object) {
				FGCEntry& Entry = Object->GCEntry;

				if (Entry.HasFlags(EGCF_Constructing)) {
					IGCObject* Prev = (IGCObject*)Tls().Get();
					
					Tls().Set(Object);
					Object->OnInitialize();
					Tls().Set(Prev);

					Entry.UnsetFlags(EGCF_Constructing);
					FGCRoot::Get().Register(Object);
				}
			}

			void IGCObject::Destroy() 
			{
				if (GCEntry.IsKillPending() ||
					GCEntry.IsDestroying())
				{
					BOX_BREAK();
					return;
				}

				GCEntry.SetFlags(EGCF_Destroying);
				GCEntry.NotifyDestroying();

				OnDestroy();
				GCEntry.SetFlags(EGCF_KillPending);
			}

			void FGCEntry::SetUpper(IGCObject* Object) { 
				if (GCUpper) {
					FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(GCUpper);
					FBarriorScope Guard(FGCRoot::Get().GetBarrior());

					Entry->GCChild.Remove(GCSelf);
				}

				GCUpper = Object; 

				if (Object) {
					FGCEntry* Entry = IGCObject::INTERNALS::GetEntry(Object);
					FBarriorScope Guard(FGCRoot::Get().GetBarrior());

					Entry->GCChild.Remove(GCSelf);
				}
			}

			void FGCEntry::NotifyDestroying()
			{
				TArray<IGCObject*> GCObjects;
				FBarriorScope Guard(FGCRoot::Get().GetBarrior());

				for (IGCObject* Object : GCChild) {
					GCObjects.Add(Object);
				}

				for (IGCObject* Object : GCObjects) {
					Object->GCEntry.SetUpper(nullptr);
				}
			}
		}
	}
}
