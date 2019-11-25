#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{
	namespace memory
	{
		template<size_t BlockSize, size_t StorageSize>
		class TMemoryPool
		{
		public:
			TMemoryPool()
				: Availables(StorageSize), Frees(0)
			{

			}

		private:
			u8 Storage[BlockSize * StorageSize];
			size_t Availables, Frees;

		public:
			FASTINLINE bool IsFull() const { return Availables <= 0; }
			FASTINLINE bool IsWasteful() const { return Frees >= StorageSize; }

		public:
			FASTINLINE bool Reset()
			{
				if (IsWasteful())
				{
					Availables = StorageSize;
					Frees = 0;
					return true;
				}

				return false;
			}

		public:
			FASTINLINE u8* Alloc()
			{
				if (!IsFull())
				{
					return Storage + (--Availables) * BlockSize;
				}

				return nullptr;
			}

			FASTINLINE bool Free(u8* Block)
			{
				if ((size_t) Storage <= (size_t) Block &&
					(size_t)Block < (size_t) (Storage + BlockSize * StorageSize))
				{
					++Frees;
					return true;
				}

				return false;
			}
		};


	}
}