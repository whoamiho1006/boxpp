#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/systems/Debugger.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

namespace boxpp
{
	/*
		Object pool, Memory pool and Fast pools.
		These objects should be declared as static member or global field.
	*/
	template<typename Type, u16 _Unitsize = 8>
	class TObjpool
	{
	private:
		class Pool;
#if BOX_DEBUG
		static constexpr u32 OBJ_Magic = 0xCAFEEFAC;
#endif

		struct Obj
		{
#if BOX_DEBUG
			u32 Magic;
#endif
			Pool* Source;
			u8 Storage[sizeof(Type)];
		};

		class Pool
		{
		public:
			Pool() : Cursor(0), Decursor(0) { }

		private:
			FAtomicBarrior Atomic;
			Obj Storage[_Unitsize];
			u16 Cursor, Decursor;

		public:
			FASTINLINE bool IsEmpty() const { return Cursor == Decursor; }
			FASTINLINE bool CanAlloc() const { return Cursor < _Unitsize || IsEmpty(); }

		public:
			FASTINLINE Type* Alloc() {
				FAtomicScope Guard(Atomic);
				Obj* Mem = nullptr;

				if (IsEmpty()) {
					Cursor = Decursor = 0;
				}

				if (Cursor < _Unitsize) {
					(Mem = Storage + Cursor)
						->Source = this;

#if BOX_DEBUG
					Mem->Magic = OBJ_Magic;
#endif
					++Cursor;
				}

				return (Type*)(Mem ? Mem->Storage : nullptr);
			}

			FASTINLINE bool Free(Type* Mem) {
				FAtomicScope Guard(Atomic);
				if (Decursor < _Unitsize) {
					if ((size_t)Storage <= (size_t)Mem &&
						(size_t)(Storage + _Unitsize) > (size_t)Mem)
					{
						++Decursor;

						if (IsEmpty()) {
							Cursor = Decursor = 0;
						}

						return true;
					}
				}

				return false;
			}
		};

	private:
		FAtomicBarrior Atomic;
		Pool* CurrentPool;
		bool bDestroying;

	public:
		TObjpool()
			: CurrentPool(nullptr), bDestroying(false)
		{
		}

		~TObjpool() {
			FAtomicScope Guard(Atomic);
			bDestroying = true;

			if (CurrentPool) {
				if (CurrentPool->IsEmpty())
					delete CurrentPool;

				CurrentPool = nullptr;
			}
		}

	public:
		FASTINLINE Type* Alloc() {
			FAtomicScope Guard(Atomic);
			Type* Mem = nullptr;

			while (!Mem) {
				if (!CurrentPool || 
					!CurrentPool->CanAlloc()) 
				{
					CurrentPool = new Pool();
					BOX_ASSERT(CurrentPool != nullptr,
						"Object pool couldn't allocate memory!");
				}

				Mem = CurrentPool->Alloc();
				if (!CurrentPool->CanAlloc()) {
					CurrentPool = nullptr;
				}
			}

			return Mem;
		}

		FASTINLINE bool Free(Type* Mem) {
			if (Mem) {
				Obj* Header = ((Obj*)Mem - 1);

#if BOX_DEBUG
				if (Header->Magic == OBJ_Magic)
#endif
				{
					FAtomicScope Guard(Atomic);

					if (Pool* Pool = Header->Source) {
						bool bRetVal = Pool->Free(Mem);

						if (Pool != CurrentPool &&
							Pool->IsEmpty()) 
						{
							if (CurrentPool || bDestroying)
								delete Pool;

							else CurrentPool = Pool;
						}
					}

					else if (CurrentPool)
						return CurrentPool->Free(Mem);
				}
			}

			return false;
		}
	};
}