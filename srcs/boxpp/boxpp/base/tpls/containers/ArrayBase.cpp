#include "ArrayBase.hpp"

namespace boxpp {
	namespace _ {
		BOXPP FFastpool* GetArrayPool()
		{
			static FFastpool _Pool(2048);
			return &_Pool;
		}
	}
}