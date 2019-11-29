#include "ObjectInitializer.hpp"

namespace boxpp{
	FRawTls& FObjectInitializer::Tls()
	{
		static FRawTls _Tls;
		return _Tls;
	}

	FObjectInitializer::FObjectInitializer(EObjectInitType InitType)
		: InitType(InitType), Object(nullptr), ObjectUpper(nullptr)
	{
		UpperInit = (FObjectInitializer*)Tls().Get();
		ObjectUpper = UpperInit ? UpperInit->GetObject() : nullptr;

		Tls().Set(this);
	}

	FObjectInitializer::~FObjectInitializer()
	{
		Tls().Set(UpperInit);

		ObjectUpper = nullptr;
		UpperInit = nullptr;
	}
}
