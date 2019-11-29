#include "Object.hpp"
#include <boxpp/core/rtti/Class.hpp>

namespace boxpp
{
	XObject* XObject_DefaultConstruct(const FObjectInitializer& Init) {
		return new XObject(Init);
	}

	XClass* XObject::StaticClass()
	{
		static XClass* ClassType = nullptr;
		static XClass ClassBody;
		static XObject ClassDefaultObject;

		if (!ClassType) {
			FClassImplementator Impl(ClassType = &ClassBody, true);

			/* Set Name and C++ Name, Namespace. */
			Impl.SetName(BOXTEXT("Box"));
			Impl.SetCppName(BOXTEXT("boxpp::XBox"));
			Impl.SetNamespace(BOXTEXT("boxpp"));

			/* Set super class to self. */
			Impl.SetSuperClass(ClassType);

			/* Set class default object.*/
			ClassDefaultObject.SetFlags(EOFLAG_ClassDefaultObject | EOFLAG_RootSet);
			Impl.SetClassDefaultObject(&ClassDefaultObject);

			/* Set constructor. */
			Impl.SetConstructor(XObject_DefaultConstruct);
		}

		return ClassType;
	}
}