#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp {
#if BOX_COMPILE_BODY
	class FThreading;
#endif

	/*
		IThreaded interface.
		This interface provides common interface that can be THREADED.
	*/
	class IThreaded
	{
	public:
		friend class FThreading;

	public:
		IThreaded() : RawHandle(nullptr) { }
		virtual ~IThreaded() { }

#if BOX_COMPILE_BODY
	protected:
		class Deploy {
		protected:
			friend class FThreading;
			static void Configure(IThreaded* Threaded);
		};
#endif

	public:
		FASTINLINE void* GetRaw() const { return RawHandle; }

	protected:
		virtual void OnRun() = 0;

	private:
		void* RawHandle;
	};

}