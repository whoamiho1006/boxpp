#ifndef __BOXPP_IBOX_HPP__
#define __BOXPP_IBOX_HPP__

#ifndef __BOXPP_HPP__
/* Loads boxpp.hpp header if not loaded. */
#include <boxpp.hpp>
#endif

#ifndef __BOXPP_SHAREDPTR_BORADCASTED_HPP__
#include <boxpp/sharedptr/BroadcastedObject.hpp>
#endif

namespace boxpp
{
	class IAllocator;

	/*
		IBox interface.

		`Box` objects hold all instances that created for each modules and,
		this works like factory which construct all object implemented in this library.
	*/
	class IBox : public IBroadcastedObject<IBox, ESharedMode::Safe>
	{
	public:
		virtual ~IBox() { }

	public:
		/* Memory allocator for this box. */
		virtual IAllocator* GetAllocator() = 0;
	};

}

#endif