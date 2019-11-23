#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>


namespace boxpp
{
	class BOXPP FDebugger
	{
	private:
		static s32 bPresent;

	public:
		static bool IsPresent();
		static void Break();
		static void Abort();
	};
}