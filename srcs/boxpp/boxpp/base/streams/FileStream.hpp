#pragma once
#include <boxpp/base/streams/Stream.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>

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
		virtual ~FFileStream();

	public:
		virtual bool IsValid() const { return File; }
		virtual bool IsLocked() const { return File && bLocked > 0; }

	public:
		virtual bool HasCapability(EStreamCaps Capability) const { return File != nullptr; }

	public:
		virtual ssize_t Tell() const;
		virtual ssize_t Seek(ssize_t Offset, EStreamSeek Origin);

		virtual ssize_t Read(void* Buffer, size_t Size);
		virtual ssize_t Write(const void* Buffer, size_t Size);

		virtual bool Flush(bool bSync = true);
		virtual bool Close();

	private:
		void* File; s32 bLocked;
		FAtomicBarrior Atomic;
		
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