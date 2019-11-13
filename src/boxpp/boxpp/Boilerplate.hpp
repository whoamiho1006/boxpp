#ifndef __BOXPP_BOILERPLATE_HPP__
#define __BOXPP_BOILERPLATE_HPP__

#include <boxpp/IBox.hpp>

namespace boxpp {
	namespace boilerplates {

		class IBoilerplate
		{
		public:
			virtual ~IBoilerplate() { }

		public:
			virtual void* Alloc(size_t Size) = 0;
			virtual void* Realloc(void* Block, size_t NewSize) = 0;
			virtual void Free(void* Block) = 0;
		};

		/*	Boilerplate that manages the allocator 
			which used for allocating memory in the entire library.
			boilerplate should be configured before calling `bxExecRuntime(...)` method.
			and, unconfigured before calling `bxLeaveRuntime(...)` method. */
		class BOXPP FBoilerplate {
		private:
			static IBoilerplate* Plate;

		public:
			static IBoilerplate* Get();

			/* Internal use only! */
			static void Set(IBoilerplate* Boilerplate);

		public:
			static void* Alloc(size_t Size);
			static void* Realloc(void* Block, size_t NewSize);
			static void Free(void* Block);
		};
	}
}
#endif // !__BOXPP_BOILERPLATE_HPP__