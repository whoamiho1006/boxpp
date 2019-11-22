#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/core/Barrior.hpp>
#include <boxpp/memory/MemoryPool.hpp>

namespace boxpp
{
	template<typename SelfType, size_t BlockSize = 32>
	class TPooledObject
	{
	public:
		using PoolType = memory::TMemoryPool<
			sizeof(void*) + sizeof(SelfType), BlockSize>;

	private:
		static FBarrior& GetBarrior() {
			static FBarrior Barrior;
			return Barrior;
		}

		static PoolType** GetPool() {
			static PoolType* Pool = nullptr;
			return &Pool;
		}

		FASTINLINE static bool IsPoolFull() {
			PoolType* Pool = *GetPool();
			return !Pool || Pool->IsFull();
		}

	public:
		FASTINLINE static void* operator new (size_t) {
			FBarriorScope Guard(GetBarrior());
			PoolType* Pool = *GetPool();
			u8* Alloc = nullptr;

			if (IsPoolFull()) {
				(Pool = *GetPool()) = new PoolType();
			}

			*((PoolType*)(Alloc = Pool->Alloc())) = Pool;
			return Alloc + sizeof(PoolType*);
		}

		FASTINLINE static void operator delete(void* Block) {
			FBarriorScope Guard(GetBarrior());
			PoolType* GlobalPool = *GetPool();

			if (Block) {
				PoolType* Pool = ((PoolType*)Block - 1);
				Pool->Free((PoolType*)Block - 1);

				if (Pool->IsWasteful()) {
					Pool->Reset();

					if (IsPoolFull()) {
						*GetPool() = Pool;
					}

					else delete Pool;
				}
			}
		}
	};
}