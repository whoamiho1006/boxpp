#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	namespace internals {
		FASTINLINE u64 hashDjb2(const void* Data, size_t Length)
		{
			const u8* bData = (const u8*)Data;
			u64 hash = 5381;
			s32 c = 0;

			if (bData) {
				while (Length) {
					c = *(bData++);
					hash = ((hash << 5) + hash) + c;
					--Length;
				}
			}

			return hash;
		}
	}
}
