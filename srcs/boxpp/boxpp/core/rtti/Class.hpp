#pragma once
#include <boxpp/base/Base.hpp>
#include <boxpp/core/Object.hpp>

namespace boxpp
{
	class FClassImplementator;
	class BOXPP XClass : public XObject
	{
		friend class FClassImplementator;

	public:
		static XClass* StaticClass();

	public:
		XClass();

		/* Class Default Object Constructor. */
		XClass(const FObjectInitializer& Init);

	public:
		/* Get class type of this object. */
		virtual XClass* GetClass() const { return StaticClass(); }

	protected:
		FName Name, CppName;
		FName Namespace;

		XClass* SuperClass;
		const XObject* ClassDefaultObject;
		TCallback<XObject*(const FObjectInitializer&)> Constructor;

	public:
		/* Get name of this class. */
		FASTINLINE const FName& GetName() const { return Name; }

		/* Get C++ name of this class. */
		FASTINLINE const FName& GetCppName() const { return CppName; }

		/* Get C++ namespace of this class.*/
		FASTINLINE const FName& GetNamespace() const { return Namespace; }

		/* Get super class of this object. */
		FASTINLINE XClass* GetSuperClass() const { return SuperClass; }

		/* Get class default object. */
		FASTINLINE const XObject* GetClassDefaultObject() const { return ClassDefaultObject; }

		/* Get class constructor. */
		FASTINLINE const TCallback<XObject*(const FObjectInitializer&)>& GetConstructor() const { return Constructor; }

		/* Determines this class is subclass of given class or not. */
		bool IsSubclassOf(const XClass* ClassType) const;

		/* Determines given object is based on this class pr not.*/
		FASTINLINE bool IsInstanceOf(const XObject* Box) const {
			return Box && Box->GetClass()->IsSubclassOf(this);
		}
	};

	/*
		Class Implementator.
	*/
	class BOXPP FClassImplementator
	{
	public:
		FClassImplementator(XClass* ClassType, bool bNoDefaultSuper = false);
		FClassImplementator(XClass* ClassType, XClass* BaseType);

	private:
		XClass* ClassType;

	public:
		FASTINLINE void SetName(const FName& Name) {
			ClassType->Name = Name;
		}

		FASTINLINE void SetCppName(const FName& Name) {
			ClassType->CppName = Name;
		}

		FASTINLINE void SetNamespace(const FName& Name) {
			ClassType->Namespace = Name;
		}

		FASTINLINE void SetSuperClass(XClass* ClassType) {
			this->ClassType->SuperClass = ClassType;
		}

		FASTINLINE void SetClassDefaultObject(XObject* Instance) {
			ClassType->ClassDefaultObject = Instance;
		}

		FASTINLINE void SetConstructor(TCallback<XObject*(const FObjectInitializer&)> Constructor) {
			ClassType->Constructor = Forward< TCallback<XObject*(const FObjectInitializer&)>>(Constructor);
		}
	};
}