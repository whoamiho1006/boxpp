#pragma once
#include <boxpp/base/Base.hpp>

namespace boxpp {

	enum class EObjectInitType
	{
		NormalObject = 0,
		ClassDefaultObject
	};

	class XObject;
	class BOXPP FObjectInitializer
	{
		friend class XObject;

	private:
		static FRawTls& Tls();

	public:
		FObjectInitializer(EObjectInitType InitType);
		~FObjectInitializer();

	private:
		FObjectInitializer* UpperInit;
		const EObjectInitType InitType;
		mutable XObject* Object;
		XObject* ObjectUpper;

	public:
		FASTINLINE EObjectInitType GetInitType() const { return InitType; }

	public:
		FASTINLINE XObject* GetObject() const { return Object; }
		FASTINLINE XObject* GetUpperObject() const {
			if (ObjectUpper) return ObjectUpper;
			return UpperInit ? UpperInit->GetObject() : nullptr;
		}

	public:
		FASTINLINE void SetUpperObject(XObject* UpperObject) {
			ObjectUpper = UpperObject;
		}

	protected:
		/* Called by XObject's 1st initializer. */
		FASTINLINE void SetObject(XObject* Object) const { this->Object = Object; }

	};

}