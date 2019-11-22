#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

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

#define BOXPP_DECLARE_BOILERPLATE() \
	void* operator new (boxpp::size_t Size) { return boxpp::boilerplates::FBoilerplate::Alloc(Size); } \
	void* operator new[] (boxpp::size_t Size) { return boxpp::boilerplates::FBoilerplate::Alloc(Size); } \
	void operator delete(void* Block) { boxpp::boilerplates::FBoilerplate::Free(Block); } \
	void operator delete[](void* Block) { boxpp::boilerplates::FBoilerplate::Free(Block); }

	}
}
