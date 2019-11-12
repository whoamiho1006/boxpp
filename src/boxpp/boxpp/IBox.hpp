#ifndef __BOXPP_IBOX_HPP__
#define __BOXPP_IBOX_HPP__

#ifndef __BOXPP_HPP__
/* Loads boxpp.hpp header if not loaded. */
#include <boxpp.hpp>
#endif

namespace boxpp
{
	/*
		IBox interface.

		`Box` objects hold all instances that created for each modules and,
		this works like factory which construct all object implemented in this library.
	*/
	class IBox
	{
	public:
		virtual ~IBox() { }

	public:

	};

}

#endif