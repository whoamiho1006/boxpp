#pragma once
#include <boxpp/base/Base.hpp>
#include <boxpp/core/Object.hpp>

namespace boxpp
{
	class FPropertyImplementator;
	class BOXPP XProperty : public XObject
	{
		friend class FPropertyImplementator;

	public:
		static XClass* StaticClass();

	public:
		XProperty();

		/* Class Default Object Constructor. */
		XProperty(const FObjectInitializer& Init);

	public:
		/* Get class type of this object. */
		virtual XClass* GetClass() const { return StaticClass(); }

		/* Get inner class type. */
		virtual XClass* GetInnerClass() const { return nullptr; }

		/* Get size of the property. */
		virtual size_t GetSizeInBytes() const { return 0; }

	};

	class BOXPP FPropertyImplementator
	{

	};
}