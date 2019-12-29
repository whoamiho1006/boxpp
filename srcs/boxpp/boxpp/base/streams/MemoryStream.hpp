#pragma once
#include <boxpp/base/streams/Stream.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

#include <boxpp/base/tpls/traits/Movable.hpp>

namespace boxpp
{
	class BOXPP FMemoryStream : public IStream
	{
	private:
		enum {
			MEMFL_READ_ONLY = 1,
			MEMFL_NO_DELETE = 2
		};

	public:
		FMemoryStream(size_t InitialCapacity = 0, ssize_t MaxSize = -1);
		FMemoryStream(void* Memory, size_t Size, bool bReadOnly = false, bool bNoDelete = false);
		FMemoryStream(const FMemoryStream& Other) = delete;

		FASTINLINE FMemoryStream(FMemoryStream&& Other)
			: Memory(nullptr), Capacity(0), Offset(0), MaxSize(0),
			  Flags(0), bLocked(0), ErrorCode(EStreamError::Success)
		{
			FAtomicScope __Guard(Other.Atomic);
			Swap(Memory, Other.Memory);

			Swap(Capacity, Other.Capacity);
			Swap(Offset, Other.Offset);
			Swap(MaxSize, Other.MaxSize);

			Swap(Flags, Other.Flags);
			Swap(bLocked, Other.bLocked);
			Swap(ErrorCode, Other.ErrorCode);
		}

		virtual ~FMemoryStream();

	private:
		u8* Memory;

		size_t Capacity, Offset;
		ssize_t MaxSize;

		u8 Flags; s32 bLocked;
		FAtomicBarrior Atomic;
		mutable EStreamError ErrorCode;

	public:
		FMemoryStream& operator =(const FMemoryStream& Other) = delete;
		FASTINLINE FMemoryStream& operator =(FMemoryStream&& Other) {
			if (this != &Other) {
				FAtomicScope __Guard(Atomic);
				FAtomicScope __Guard(Other.Atomic);

				Swap(Memory, Other.Memory);

				Swap(Capacity, Other.Capacity);
				Swap(Offset, Other.Offset);
				Swap(MaxSize, Other.MaxSize);

				Swap(Flags, Other.Flags);
				Swap(bLocked, Other.bLocked);
				Swap(ErrorCode, Other.ErrorCode);
			}

			return *this;
		}

	public:
		virtual bool IsValid() const { return Memory; }
		virtual bool IsLocked() const { return Memory && bLocked > 0; }

		virtual EStreamError GetErrorCode() const { return ErrorCode; }

	public:
		virtual bool IsEndOfStream() const { return !Memory || Offset >= Capacity; }

	public:
		virtual bool HasCapability(EStreamCaps Capability) const;
		FASTINLINE size_t GetCapacity() const { return Memory ? Capacity : 0; }

		FASTINLINE ssize_t GetMaxSize() const { return MaxSize; }
		FASTINLINE void SetMaxSize(ssize_t Size) { MaxSize = Size; }

		FASTINLINE u8* GetRaw() const { return Memory; }

		virtual bool Resize(size_t NewCapacity);

	public:
		virtual ssize_t Tell() const;
		virtual ssize_t Seek(ssize_t Offset, EStreamSeek Origin);

		virtual ssize_t Read(void* Buffer, size_t Size);
		virtual ssize_t Write(const void* Buffer, size_t Size);

		virtual bool Flush(bool bSync = true) { return true; }
		virtual bool Close();

	private:
		FASTINLINE void SetLocked() {
			FAtomicScope Guard(Atomic);
			if (Memory != nullptr) {
				++bLocked;
			}
		}

		FASTINLINE void SetUnlocked() {
			FAtomicScope Guard(Atomic);
			if (--bLocked < 0) {
				bLocked = 0;
			}
		}
	};
}