#pragma once
#include <boxpp/base/Base.hpp>

namespace boxpp
{
	class XClass;

	enum EObjectFlags
	{
		EOFLAG_DynamicObject		= 0x0001,
		EOFLAG_ClassDefaultObject	= 0x0002,
		EOFLAG_RootSet				= 0x0004,
		EOFLAG_MarkPendingKill		= 0x0008
	};

	struct FObjectBaseHelper;

	/*
		Object Base.
		
	*/
	class BOXPP XObjectBase
	{
		friend struct FObjectBaseHelper;

	public:
		/* Internal Constructor. */
		XObjectBase();

	protected:
		virtual ~XObjectBase();

	protected:
		u32 Flags;
		XObjectBase* Upper;

	public:
		/* Get class type of this object. */
		virtual XClass* GetClass() const = 0;

	public:
		FASTINLINE bool HasFlags(u32 Flags) {
			return (this->Flags & Flags) == Flags;
		}

		FASTINLINE bool HasAnyFlags(u32 Flags) {
			return (this->Flags & Flags) != 0;
		}

	protected:
		FASTINLINE void SetFlags(u32 Flags) {
			this->Flags |= Flags;
		}

		FASTINLINE void UnsetFlags(u32 Flags) {
			this->Flags = this->Flags & (~Flags);
		}
	};

	struct FObjectBaseHelper
	{
		FASTINLINE static void MarkDynamicObject(XObjectBase* Object) {
			Object->SetFlags(EOFLAG_DynamicObject);
		}
	};
}