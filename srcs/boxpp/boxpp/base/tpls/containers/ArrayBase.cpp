#include "ArrayBase.hpp"

namespace boxpp {
#if BOX_ENFORCE_GLOBAL_POOL
	FFastpool& GetGlobalPool();
#endif

	namespace _ {
		BOXPP FFastpool* GetArrayPool()
		{
#if BOX_ENFORCE_GLOBAL_POOL
			return &GetGlobalPool();
#else
			static FFastpool _Pool(BOX_ARRAY_POOL_ALIGNMENT);
			return &_Pool;
#endif
		}
	}
}