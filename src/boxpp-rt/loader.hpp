#pragma once
#include <boxpp.hpp>
#include <boxpp/internal/IBoxRuntime.hpp>

/*
namespace boxpp_rt {
	typedef struct { void* unused; } *OBoxLibrary;

	struct SBoxInterface {
		OBoxLibrary			Handle;
		const boxpp::c8*	Name;

		FAR_bxEnterRuntime	Enter;
		FAR_bxLeaveRuntime	Leave;

		FAR_bxExecRuntime	Exec;
	};

	bool bxLoadInterface(SBoxInterface& Interface);
	void bxUnloadInterface(SBoxInterface& Interface);
}
*/