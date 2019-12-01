#include "Property.hpp"
#include <boxpp/core/rtti/Class.hpp>

namespace boxpp {
	XClass* XProperty::StaticClass()
	{
		static XClass* ClassType = nullptr;
		static XClass ClassBody;
		static XProperty ClassDefaultObject;

		if (!ClassType) {
			FClassImplementator Impl(ClassType = &ClassBody, true);

			/* Set Name and C++ Name, Namespace. */
			Impl.SetName(BOXTEXT("XProperty"));
			Impl.SetCppName(BOXTEXT("boxpp::XProperty"));
			Impl.SetNamespace(BOXTEXT("boxpp"));

			/* Set super class to object. */
			Impl.SetSuperClass(XObject::StaticClass());

			/* Set class default object.*/
			ClassDefaultObject.SetFlags(EOFLAG_ClassDefaultObject | EOFLAG_RootSet);
			Impl.SetClassDefaultObject(&ClassDefaultObject);
		}

		return ClassType;
	}

	XProperty::XProperty()
	{

	}

	XProperty::XProperty(const FObjectInitializer& Init)
	{
	}
}