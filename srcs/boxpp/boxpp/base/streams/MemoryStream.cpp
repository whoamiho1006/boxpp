#include "MemoryStream.hpp"

#include <boxpp/base/boilerplates/Memory.hpp>

namespace boxpp {
	FMemoryStream::FMemoryStream(size_t InitialCapacity, ssize_t MaxSize)
		: Memory(0), Capacity(0), Offset(0), MaxSize(MaxSize), Flags(0), bLocked(0)
	{
		ErrorCode = EStreamError::Success;
		if (InitialCapacity) {
			if (MaxSize > 0 && InitialCapacity > size_t(MaxSize)) {
				InitialCapacity = MaxSize;
			}

			if (InitialCapacity) {
				Memory = (u8*)FMemory::Malloc(InitialCapacity);
				Capacity = InitialCapacity;
			}
		}
	}

	FMemoryStream::FMemoryStream(void * Memory, size_t Size, bool bReadOnly, bool bNoDelete)
		: Memory((u8*)Memory), Capacity(Size), Offset(Size), MaxSize(bReadOnly || bNoDelete ? ssize_t(Size) : -1),
		  Flags((bReadOnly ? MEMFL_READ_ONLY : 0) | (bNoDelete ? MEMFL_NO_DELETE : 0)), bLocked(0)
	{
		ErrorCode = EStreamError::Success;
	}

	FMemoryStream::~FMemoryStream() {
		FAtomicScope Guard(Atomic);

		if (Memory && !(Flags & MEMFL_NO_DELETE)) {
			FMemory::Free(Memory);
		}
	}

	bool FMemoryStream::HasCapability(EStreamCaps Capability) const {
		if (Memory) {
			switch (Capability) {
			case EStreamCaps::Read:
			case EStreamCaps::Seek:
				return true;

			case EStreamCaps::Write:
				return !(Flags & MEMFL_READ_ONLY);
			}
		}

		return false;
	}

	bool FMemoryStream::Resize(size_t NewCapacity)
	{
		FAtomicScope Guard(Atomic);

		if (!(Flags & MEMFL_READ_ONLY) && 
			!(Flags & MEMFL_NO_DELETE))
		{
			if (!NewCapacity) {
				return Close();
			}

			if (!Memory) {
				Memory = (u8*)FMemory::Malloc(Capacity = NewCapacity);
			}

			else {
				Memory = (u8*)FMemory::Realloc(Memory, Capacity = NewCapacity);
			}

			if (Offset > Capacity) {
				Offset = Capacity;
			}

			ErrorCode = EStreamError::Success;
			return true;
		}

		ErrorCode = EStreamError::NotSupported;
		return false;
	}

	ssize_t FMemoryStream::Tell() const
	{
		FAtomicScope Guard(Atomic);
		ErrorCode = EStreamError::Success;

		if (Memory) {
			return ssize_t(Offset);
		}

		return 0;
	}

	ssize_t FMemoryStream::Seek(ssize_t _Offset, EStreamSeek Origin)
	{
		FAtomicScope Guard(Atomic);
		ErrorCode = EStreamError::Success;

		if (Memory) {
			switch (Origin) {
			case EStreamSeek::Begin:
				Offset = size_t(_Offset <= 0 ? 0 : _Offset);
				break;

			case EStreamSeek::Current:
				Offset = size_t(ssize_t(Offset) + _Offset <= 0 ? 
							0 : ssize_t(Offset) + _Offset);
				break;

			case EStreamSeek::End:
				Offset = size_t(ssize_t(Capacity) + _Offset <= 0 ?
							0 : ssize_t(Capacity) + _Offset);
				break;
			}

			if (Offset > Capacity) {
				Offset = Capacity;
			}

			return ssize_t(Offset);
		}

		return 0;
	}

	ssize_t FMemoryStream::Read(void* Buffer, size_t Size)
	{
		ErrorCode = EStreamError::Success;

		if (Memory) {
			bool bSizeZero = !Size;
			SetLocked();

			if (Offset + Size > Capacity) {
				Size = Capacity - Offset;
			}

			if (Size) {
				memcpy(Buffer, Memory + Offset, Size);
				Offset += Size;
			}

			if (!bSizeZero && !Size) {
				ErrorCode = EStreamError::Overflow;
			}
			
			SetUnlocked();
			return ssize_t(Size);
		}

		return 0;
	}

	ssize_t FMemoryStream::Write(const void* Buffer, size_t Size)
	{
		ErrorCode = EStreamError::Success;

		if (!(Flags & MEMFL_READ_ONLY)) {
			if (Offset + Size <= 0 || !Size) {
				return 0;
			}

			SetLocked();

			if (Size && Offset + Size > Capacity)
			{
				if (Flags & MEMFL_NO_DELETE)
				{
					Size = Capacity - Offset;
				}
			
				else if (MaxSize >= 0 &&
					Offset + Size > size_t(MaxSize))
				{
					if (Offset < size_t(MaxSize))
						Size = MaxSize - Offset; 
					else Size = 0;
				}

				else if (!Memory) {
					Memory = (u8*)FMemory::Malloc(Capacity = Offset + Size);
				}

				else {
					Memory = (u8*)FMemory::Realloc(Memory, Capacity = Offset + Size);
				}

				if (!Size) {
					ErrorCode = EStreamError::NoSpace;
				}
			}

			if (Size) {
				memcpy(Memory + Offset, Buffer, Size);
				Offset += Size;
			}

			SetUnlocked();
			return ssize_t(Size);
		}

		return 0;
	}

	bool FMemoryStream::Close()
	{
		FAtomicScope Guard(Atomic);
		ErrorCode = EStreamError::Success;

		if (Memory) {
			if (!(Flags & MEMFL_NO_DELETE)) {
				FMemory::Free(Memory);
			}

			Memory = nullptr;
			Capacity = Offset = 0;
			MaxSize = 0; Flags = 0;
			bLocked = 0;
			return true;
		}

		return false;
	}
}