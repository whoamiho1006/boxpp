#pragma once
#include <boxpp/base/Base.hpp>

namespace boxpp
{
	namespace core
	{
		namespace gc
		{
			class FGCRoot;
			class IGCObject;

			enum EGCFlags
			{
				EGCF_Constructing	= 0x01,
				EGCF_KillPending	= 0x02,
				EGCF_Destroying		= 0x04,
				EGCF_RootSet		= 0x08
			};

			class BOXPP FGCEntry
			{
				friend class FGCRoot;
				friend class IGCObject;

			public:
				FGCEntry()
					:	GCFlags(EGCF_Constructing), 
						GCUpper(nullptr),
						GCSelf(nullptr)
				{
				}

				~FGCEntry()
				{
				}

			private:
				u32 GCFlags;

				IGCObject* GCUpper;
				IGCObject* GCSelf;

				TSortedArray<IGCObject*> GCChild;

			public:
				/* Determines this object is marked as Kill-pending or not. (by GC) */
				FASTINLINE bool IsKillPending() const { return GCFlags & EGCF_KillPending; }

				/* Determines this object is marked as Destroying now or not. */
				FASTINLINE bool IsDestroying() const { return GCFlags & EGCF_Destroying; }

				/* Determines this object is unreachable anyway. */
				FASTINLINE bool IsUnreachableAnyway() const { return HasAnyFlags(EGCF_KillPending | EGCF_Destroying); }

			public:
				FASTINLINE void SetFlags(u32 Flags) { 
					BOX_ASSERT(!(GCFlags & EGCF_Constructing) && (Flags & EGCF_RootSet),
						"Already created object cannot be a rootset!");

					GCFlags |= Flags; 
				}

				FASTINLINE void UnsetFlags(u32 Flags) {
					BOX_ASSERT(!(GCFlags & EGCF_Constructing) && (Flags & EGCF_RootSet),
						"Already created object cannot be a non-rootset!");

					GCFlags = GCFlags & ~Flags; 
				}

				FASTINLINE bool HasFlags(u32 Flags) const { return (GCFlags & Flags) == Flags; }
				FASTINLINE bool HasAnyFlags(u32 Flags) const { return (GCFlags & Flags) != 0; }

			public:
				/* Set upper object entity. */
				void SetUpper(IGCObject* Object);
				void NotifyDestroying();

				/* Get upper object entity. */
				FASTINLINE IGCObject* GetUpper() const { return GCUpper; }

			protected:
				FASTINLINE void SetSelf(IGCObject* Object) { GCSelf = Object; }
			};

			/*
				IGCObject.
				Base class of GC Objects.
			*/
			class BOXPP IGCObject
			{
				friend class FGCRoot;
				friend class FGCEntry;

			private:
				static FRawTls& Tls();

			protected:
				class INTERNALS
				{
					friend class FGCRoot;
					friend class FGCEntry;

				protected:
					FASTINLINE static FGCEntry* GetEntry(const IGCObject* Object) {
						return &Object->GCEntry;
					}

					static void Initialize(IGCObject* Object);
				};

			public:
				IGCObject();
				IGCObject(IGCObject* Upper);
				virtual ~IGCObject();

			private:
				mutable FGCEntry GCEntry;

			public:
				/* Determines this object's status is valid or not. */
				FASTINLINE bool IsValid() const { return !GCEntry.IsUnreachableAnyway(); }

			protected:
				/* Initialize. */
				virtual void OnInitialize() { }

				/* Destroying. */
				virtual void OnDestroy() { }

			public:
				/* Destroy this object. */
				virtual void Destroy();
			};
		}
	}
}