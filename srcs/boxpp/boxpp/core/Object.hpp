#pragma once
#include <boxpp/base/Base.hpp>
#include <boxpp/core/ObjectBase.hpp>
#include <boxpp/core/ObjectInitializer.hpp>

namespace boxpp
{
	namespace _ {
		struct FObjectInitProxy;
	}

	class XClass;
	class BOXPP XObject : public XObjectBase
	{
		friend struct _::FObjectInitProxy;

	public:
		/* Internal Constructor. */
		XObject()
		{

		}

		/* Class Default Object Constructor. */
		XObject(const FObjectInitializer& Init)
		{
			Init.SetObject(this);
			Upper = Init.GetUpperObject();
		}

	protected:
		virtual ~XObject() { }

	public:
		static XClass* StaticClass();
		
	public:
		/* Get class type of this object. */
		virtual XClass* GetClass() const override { return StaticClass(); }

	protected:
		virtual void OnInitialize() { };
	};

	// ---------------------------------
	namespace _ { struct FObjectInitProxy {
		FASTINLINE static void Initialize(XObject* Object) { Object->OnInitialize(); }
	}; }
	// ---------------------------------

	template<typename ObjectType>
	FASTINLINE ObjectType* NewObject(XClass* Class, XObject* Upper)
	{
		static_assert(IsDerivedType<XObject, ObjectType>,
			"NewObject can constuct only XObject based classes!");

		FObjectInitializer Init(EObjectInitType::NormalObject);
		ObjectType* Instance = nullptr;

		if (Upper) {
			Init.SetUpperObject(Upper);
		}

		if (!Class)
			Instance = new ObjectType(Init); 

		else if (Class->IsSubclassOf(ObjectType::StaticClass())) {
			Instance = dynamic_cast<ObjectType*>((Class->GetConstructor())(Init));
		}

		if (Instance) {
			FObjectBaseHelper::MarkDynamicObject(Instance);
			_::FObjectInitProxy::Initialize(Instance);
		}

		return Instance;
	}
}