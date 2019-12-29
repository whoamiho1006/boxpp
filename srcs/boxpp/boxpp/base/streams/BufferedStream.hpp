#pragma once
#include <boxpp/base/streams/Stream.hpp>
#include <boxpp/base/systems/AtomicBarrior.hpp>
#include <boxpp/base/streams/MemoryStream.hpp>

namespace boxpp
{
	class BOXPP FBufferedStream : public IStream
	{
	public:
		FBufferedStream();
		FBufferedStream(IStream* Stream, size_t Buffer = 16 * 1024, bool bFlushAuto = true);
		virtual ~FBufferedStream();

	public:
		virtual bool IsValid() const { return BaseStream; }
		virtual bool IsLocked() const { return BaseStream && InternalBuffer.IsLocked(); }

		virtual EStreamError GetErrorCode() const { return ErrorCode; }

	public:
		virtual bool HasCapability(EStreamCaps Capability) const { return BaseStream->HasCapability(Capability); }
		virtual IStream* GetBaseStream() const { return BaseStream; }

	public:
		virtual bool IsEndOfStream() const { 
			return	BaseStream && BaseStream->IsEndOfStream() && 
					InternalBuffer.IsEndOfStream(); 
		}

	public:
		virtual ssize_t Tell() const;
		virtual ssize_t Seek(ssize_t Offset, EStreamSeek Origin);

		virtual ssize_t Read(void* Buffer, size_t Size);
		virtual ssize_t Write(const void* Buffer, size_t Size);

		virtual bool Flush(bool bSync = true);
		virtual bool Close();

	private:
		void LoadBuffer();
		void StoreBuffer();

	private:
		size_t Buffer, Offset;
		bool bFlushAuto, bChanges;

		IStream* BaseStream;
		FMemoryStream InternalBuffer;
		mutable EStreamError ErrorCode;
	};
}