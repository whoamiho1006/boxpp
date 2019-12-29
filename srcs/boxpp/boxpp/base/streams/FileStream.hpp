#pragma once
#include <boxpp/base/streams/Stream.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

#include <boxpp/base/tpls/traits/Movable.hpp>

namespace boxpp
{
	enum class EFileMode
	{
		ReadOnly = 1,
		ReadWrite = 2,
		AlwaysNew = 3
	};

	class BOXPP FFileStream : public IStream
	{
	public:
		FFileStream();
		FFileStream(const ansi_t* Path, EFileMode Mode);
		FFileStream(const wide_t* Path, EFileMode Mode);

		FASTINLINE FFileStream(FFileStream&& Other)
			: bEndOfStream(true), File(0), bLocked(0), ErrorCode(EStreamError::Success)
		{
			FAtomicScope __Guard(Other.Atomic);
			Swap(bEndOfStream, Other.bEndOfStream);

			Swap(File, Other.File);

			Swap(bLocked, Other.bLocked);
			Swap(ErrorCode, Other.ErrorCode);
		}

		virtual ~FFileStream();

	public:
		FFileStream& operator =(const FFileStream& Other) = delete;
		FASTINLINE FFileStream& operator =(FFileStream&& Other) {
			if (this != &Other) {
				FAtomicScope __Guard1(Atomic);
				FAtomicScope __Guard2(Other.Atomic);

				Swap(bEndOfStream, Other.bEndOfStream);

				Swap(File, Other.File);

				Swap(bLocked, Other.bLocked);
				Swap(ErrorCode, Other.ErrorCode);
			}

			return *this;
		}

	public:
		virtual bool IsValid() const { return File; }
		virtual bool IsLocked() const { return File && bLocked > 0; }

		virtual EStreamError GetErrorCode() const { return ErrorCode; }

	public:
		virtual bool HasCapability(EStreamCaps Capability) const { return File != nullptr; }

	public:
		virtual bool IsEndOfStream() const { return !File || bEndOfStream; }

	public:
		virtual ssize_t Tell() const;
		virtual ssize_t Seek(ssize_t Offset, EStreamSeek Origin);

		virtual ssize_t Read(void* Buffer, size_t Size);
		virtual ssize_t Write(const void* Buffer, size_t Size);

		virtual bool Flush(bool bSync = true);
		virtual bool Close();

	private:
		bool bEndOfStream;
		void* File; s32 bLocked;
		FAtomicBarrior Atomic;
		mutable EStreamError ErrorCode;
		
	private:
		FASTINLINE void SetLocked() {
			FAtomicScope Guard(Atomic);
			if (File != nullptr) {
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