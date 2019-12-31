#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/systems/AtomicBarrior.hpp>
#include <boxpp/base/tpls/containers/Queue.hpp>

namespace boxpp
{
	/*
		Object pool, Memory pool and Fast pools.
		These objects should be declared as static member or global field.
	*/
	template<typename Type, uint32_t _Maxholds = 8>
	class TMempool
	{
	private:
		struct Header
		{
			size_t Size;
		};

	public:
		TMempool()
			: Maxholds(_Maxholds)
		{
		}

		~TMempool() {
			Header* Temp = nullptr;

			while (Queue.Dequeue(Temp)) {
				delete[]((uint8_t*)Temp);
			}
		}

	private:
		FAtomicBarrior Barrior;
		TQueue<Header*> Queue;
		u32 Maxholds;

	public:
		FASTINLINE Type* Alloc(const size_t Size) {
			FAtomicScope Guard(Barrior);

			if (Size) {
				TQueue<Header*> Avails(Forward<TQueue<Header*>>(Queue));
				Header* Temp = nullptr,* Mem = nullptr;
				
				while (Avails.Dequeue(Temp)) {
					if (!Mem && Temp->Size >= Size) {
						++Maxholds;
						Mem = Temp;
					}
					
					else {
						Queue.Enqueue(Temp);
					}
				}

				if (!Mem) {
					Mem = (Header*) new uint8_t[
						sizeof(Header) + sizeof(Type) * Size];

					if (Mem) {
						Mem->Size = Size;
					}

					if (Queue.GetSize() >= _Maxholds) {
						if (Queue.Dequeue(Temp)) {
							delete[]((uint8_t*)Temp);
							++Maxholds;
						}
					}
				}

				if (Mem)
				{
					return (Type*)(Mem + 1);
				}
			}

			return nullptr;
		}

		FASTINLINE bool Free(Type* Blk) {
			FAtomicScope Guard(Barrior);

			if (Blk) {
				Header* _Blk = (Header*)Blk - 1;

				if (Maxholds > 0) {
					--Maxholds;
					Queue.Enqueue(_Blk);
				}

				else {
					delete[]((uint8_t*)_Blk);
				}

				return true;
			}

			return false;
		}
	};

}