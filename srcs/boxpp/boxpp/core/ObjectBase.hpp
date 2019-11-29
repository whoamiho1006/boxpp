#pragma once
#include <boxpp/base/Base.hpp>

namespace boxpp
{
	class XObject;

	/*
		XObject.
		boX Object.
	*/
	class BOXPP XObjectBase
	{
		friend class XObject;

	public:
		virtual ~XObjectBase() { }

	protected:
		XObjectBase* Upper;
		TSortedArray<XObjectBase*> Children;

	protected:
		/* Get upper object. */
		FASTINLINE XObjectBase* GetUpper() const { return Upper; }

		/* Set upper object. */
		void SetUpper(XObjectBase* Object);

	protected:
		virtual void OnAttachChild(XObjectBase* Child);
		virtual void OnDetachChild(XObjectBase* Child);
	};
}