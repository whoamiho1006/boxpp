#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	class BOXPP FDebugger
	{
	private:
		static s32 bPresent;
		static s32 bUnderCLI;

	public:
		static void Init();

	public:
		static bool IsPresent();
		static bool IsUnderCLI();

	public:
		static bool Break();
		static bool Abort();

	public:
		static bool Failure(const ansi_t* File, s32 Line, 
			const ansi_t* Expression, const ansi_t* Message);
	};

#if BOX_DEBUG
#define BOX_ASSERT(Expression, Message)	\
	((Expression) || (boxpp::FDebugger::Failure(__FILE__, __LINE__, #Expression, Message)))

#define BOX_ENSURE(Expression) \
	((Expression) || (boxpp::FDebugger::Break()))

#define BOX_BREAK()	(boxpp::FDebugger::Break() || true)
#else
#define BOX_ASSERT(Expression, Message)	\
	((Expression) || (boxpp::FDebugger::Abort()))

#define BOX_ENSURE(Expression) \
	(Expression)

#define BOX_BREAK()	true
#endif
}