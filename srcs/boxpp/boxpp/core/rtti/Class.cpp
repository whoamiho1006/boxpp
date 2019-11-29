#include "Class.hpp"

namespace boxpp {
	XObject* XClass_DefaultConstruct(const FObjectInitializer& Init) {
		return new XClass(Init);
	}

	XClass* XClass::StaticClass()
	{
		static XClass* ClassType = nullptr;
		static XClass ClassBody;
		static XClass ClassDefaultObject;

		if (!ClassType) {
			FClassImplementator Impl(ClassType = &ClassBody, true);
			
			/* Set Name and C++ Name, Namespace. */
			Impl.SetName(BOXTEXT("Class"));
			Impl.SetCppName(BOXTEXT("boxpp::XClass"));
			Impl.SetNamespace(BOXTEXT("boxpp"));

			/* Set super class to object. */
			Impl.SetSuperClass(XObject::StaticClass());

			/* Set class default object.*/
			ClassDefaultObject.SetFlags(EOFLAG_ClassDefaultObject | EOFLAG_RootSet);
			Impl.SetClassDefaultObject(&ClassDefaultObject);

			/* Set constructor. */
			Impl.SetConstructor(XClass_DefaultConstruct);
		}

		return ClassType;
	}

	XClass::XClass()
		: SuperClass(nullptr), ClassDefaultObject(nullptr)
	{
	}

	XClass::XClass(const FObjectInitializer& Init)
		: XObject(Init), SuperClass(nullptr), ClassDefaultObject(nullptr)
	{
	}

	bool XClass::IsSubclassOf(const XClass* ClassType) const {
		const XClass* Current = this;

		while (Current) {
			if (Current == ClassType) {
				return true;
			}

			else if (Current == XObject::StaticClass())
				break;

			Current = Current->SuperClass;
		}

		return false;
	}
	
	FClassImplementator::FClassImplementator(XClass* ClassType, bool bNoDefaultSuper)
		: ClassType(ClassType)
	{
		if (!bNoDefaultSuper) {
			SetSuperClass(XObject::StaticClass());
		}
	}

	FClassImplementator::FClassImplementator(XClass* ClassType, XClass* BaseType)
		: FClassImplementator(ClassType)
	{
		if (BaseType)
		{
			SetSuperClass(BaseType);
		}
	}
}