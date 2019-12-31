#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/systems/Debugger.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

#include <boxpp/base/boilerplates/Memory.hpp>

namespace boxpp
{
	/*
		Object pool, Memory pool and Fast pools.
		These objects should be declared as static member or global field.
	*/
	class FFastpool
	{
#if !BOX_DISABLE_FASTPOOL
	private:
		class Pool;

#if BOX_DEBUG
		static constexpr u32 HDR_Magic = 0xEFACCAFE;
#endif
		struct Hdr
		{
#if BOX_DEBUG
			u32 Magic;
#endif
			size_t Size;
			Pool* Source;
		};

		class Pool
		{
		public:
			Pool(u8* Tail, size_t Size)
				: Tail(Tail), Cursor(0), Decursor(0), TotalSize(Size)
			{
			}

			~Pool() { }

		private:
			FAtomicBarrior Atomic;

			u8* Tail;

			size_t Cursor, Decursor;
			size_t TotalSize;

		public:
			FASTINLINE bool IsEmpty() const { return Cursor == Decursor; }
			FASTINLINE size_t GetCapacity() const { return TotalSize; }
			FASTINLINE bool CanAlloc(size_t Size) const {
				return Cursor + Size + sizeof(Hdr) <= TotalSize || IsEmpty();
			}

		public:
			FASTINLINE u8* Alloc(size_t Size) {
				FAtomicScope Guard(Atomic);
				Hdr* Mem = nullptr;

				if (Size) {
					if (Cursor == Decursor) {
						Cursor = Decursor = 0;
					}

					if (Cursor + Size + sizeof(Hdr) < TotalSize) {
						(Mem = (Hdr*)(Tail + Cursor))
							->Source = this;
#if BOX_DEBUG
						Mem->Magic = HDR_Magic;
#endif
						Mem->Size = Size;

						Cursor += Size + sizeof(Hdr);
					}
				}

				return (u8*)(Mem ? Mem + 1 : nullptr);
			}

			FASTINLINE bool Free(u8* Ptr) {
				FAtomicScope Guard(Atomic);

				if (Decursor < TotalSize) {
					if ((size_t)Tail <= (size_t)Ptr &&
						(size_t)(Tail + TotalSize) > (size_t)Ptr)
					{
						Hdr* Mem = (Hdr*)Ptr - 1;
						Decursor += Mem->Size + sizeof(Hdr);

						if (Cursor == Decursor) {
							Cursor = Decursor = 0;
						}

						return true;
					}
				}

				return false;
			}
		};

	private:
		FASTINLINE static Pool* NewPool(size_t Size) {
			Pool* New = (Pool*)(FMemory::PureMalloc(sizeof(Pool) + Size));
			return new (New) Pool((u8*)(New + 1), Size);
		}

		FASTINLINE static void DeletePool(Pool* Old) {
			(*Old).~Pool();
			FMemory::PureFree(Old);
		}

	private:
		FAtomicBarrior Atomic;

		size_t Alignment;
		Pool* CurrentPool;
		bool bDestroying;
#endif

	public:
		FFastpool(size_t Alignment = 1024)
#if !BOX_DISABLE_FASTPOOL
			: Alignment(Alignment), CurrentPool(nullptr), bDestroying(false)
#endif
		{
		}

		~FFastpool()
		{
#if !BOX_DISABLE_FASTPOOL
			FAtomicScope Guard(Atomic);

			bDestroying = true;

			if (CurrentPool) {
				if (CurrentPool->IsEmpty())
					DeletePool(CurrentPool);

				CurrentPool = nullptr;
			}
#endif
		}

	public:
		FASTINLINE void* Alloc(size_t Size) {
#if !BOX_DISABLE_FASTPOOL
			FAtomicScope Guard(Atomic);
			void* Mem = nullptr;

			while (!Mem) {
				if (!CurrentPool ||
					!CurrentPool->CanAlloc(Size))
				{
					CurrentPool = NewPool(
						((Size + sizeof(Hdr)) / Alignment +
						((Size + sizeof(Hdr)) % Alignment > 0 ? 1 : 0)) * Alignment
					);

					BOX_ASSERT(CurrentPool != nullptr,
						"Object pool couldn't allocate memory!");
				}

				Mem = CurrentPool->Alloc(Size);
			}

			return Mem;
#else
			return FMemory::PureMalloc(Size);
#endif
		}

		FASTINLINE void* Realloc(void* Mem, size_t NewSize) {
#if !BOX_DISABLE_FASTPOOL
			if (Mem) {
				Hdr* Header = ((Hdr*)Mem - 1);
#if BOX_DEBUG
				if (Header->Magic == HDR_Magic)
#endif
				{
					void* NewMem = Alloc(NewSize);

					::memcpy(NewMem, Mem, 
						Header->Size > NewSize ? 
						NewSize : Header->Size);

					Free(Mem);
					return NewMem;
				}
			}

			return nullptr;
#else
			return FMemory::PureRealloc(Mem, NewSize);
#endif
		}

		FASTINLINE bool Free(void* Mem) {
#if !BOX_DISABLE_FASTPOOL
			if (Mem) {
				Hdr* Header = ((Hdr*)Mem - 1);

#if BOX_DEBUG
				if (Header->Magic == HDR_Magic)
#endif
				{
					FAtomicScope Guard(Atomic);

					if (Pool* Pool = Header->Source) {
						bool bRetVal = Pool->Free((u8*)Mem);

						if (Pool != CurrentPool &&
							Pool->IsEmpty())
						{
							if (CurrentPool || bDestroying)
							{
								DeletePool(Pool);
							}

							else CurrentPool = Pool;
						}
					}

					else if (CurrentPool)
						return CurrentPool->Free((u8*)Mem);
				}
			}

			return false;
#else
			FMemory::PureFree(Mem);
			return true;
#endif
		}
	};
}