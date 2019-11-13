#ifndef __BOXPP_BOILERPLATE_HPP__
#define __BOXPP_BOILERPLATE_HPP__

#include <boxpp/IBox.hpp>
#include <boxpp/memory/Allocator.hpp>

namespace boxpp {

	namespace boilerplates {
		struct SAllocInfo { IAllocator* Allocator; };
	}
}
#endif // !__BOXPP_BOILERPLATE_HPP__