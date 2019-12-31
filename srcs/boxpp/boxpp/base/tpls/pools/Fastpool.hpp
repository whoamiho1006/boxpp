#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/systems/Debugger.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

namespace boxpp
{
	class FFastpool
	{
	private:
		class Pool;
		static constexpr u32 HDR_Magic = 0xEFACCAFE;

		struct Hdr
		{
			u32 Magic;
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
			FASTINLINE size_t Capacity() const { return TotalSize; }
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

						Mem->Magic = HDR_Magic;
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
			Pool* New = (Pool*)(new u8[sizeof(Pool) + Size]);
			return new (New) Pool((u8*)(New + 1), Size);
		}

		FASTINLINE static void DeletePool(Pool* Old) {
			(*Old).~Pool();
			delete[]((u8*)Old);
		}

	private:
		FAtomicBarrior Atomic;

		size_t Alignment;
		Pool* CurrentPool;

	public:
		FFastpool(size_t Alignment = 1024)
			: Alignment(Alignment), CurrentPool(nullptr)
		{
		}

		~FFastpool()
		{
			if (CurrentPool && CurrentPool->IsEmpty()) {
				DeletePool(CurrentPool);
				CurrentPool = nullptr;
			}

			BOX_ASSERT(CurrentPool == nullptr,
				"FFastpool: Memory leak detected.");
		}

	public:
		FASTINLINE void* Alloc(size_t Size) {
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
		}

		FASTINLINE void* Realloc(void* Mem, size_t NewSize) {
			if (Mem) {
				Hdr* Header = ((Hdr*)Mem - 1);

				if (Header->Magic == HDR_Magic)
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
		}

		FASTINLINE bool Free(void* Mem) {
			if (Mem) {
				Hdr* Header = ((Hdr*)Mem - 1);

				if (Header->Magic == HDR_Magic)
				{
					FAtomicScope Guard(Atomic);

					if (Pool* Pool = Header->Source) {
						bool bRetVal = Pool->Free((u8*)Mem);

						if (Pool != CurrentPool &&
							Pool->IsEmpty())
						{
							if (CurrentPool)
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
		}
	};
}