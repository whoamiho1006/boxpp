#include "BufferedStream.hpp"

namespace boxpp {
	FBufferedStream::FBufferedStream()
		: Buffer(0), Offset(0), bFlushAuto(false), BaseStream(nullptr), bChanges(false)
	{
		ErrorCode = EStreamError::Success;
	}

	FBufferedStream::FBufferedStream(IStream* Stream, size_t Buffer, bool bFlushAuto)
		: Buffer(Buffer), Offset(0), bFlushAuto(bFlushAuto), BaseStream(Stream), bChanges(false)
	{
		if (!Buffer) {
			this->Buffer = Buffer = 16 * 1024;
		}

		InternalBuffer.SetMaxSize(Buffer);
		ErrorCode = EStreamError::Success;
	}

	FBufferedStream::~FBufferedStream()
	{
		Close();
	}

	ssize_t FBufferedStream::Tell() const
	{
		if (BaseStream) {
			if (InternalBuffer.GetCapacity())
				return Offset + InternalBuffer.Tell();

			ssize_t R = BaseStream->Tell();

			if (R < 0) { 
				ErrorCode = BaseStream->GetErrorCode(); 
			}

			return R;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	ssize_t FBufferedStream::Seek(ssize_t Offset, EStreamSeek Origin)
	{
		if (BaseStream)
		{
			ssize_t RetVal = -1;

			ErrorCode = EStreamError::NotSupported;
			if (BaseStream->HasCapability(EStreamCaps::Seek)) {
				StoreBuffer();

				RetVal = BaseStream->Seek(Offset, Origin);
				ErrorCode = BaseStream->GetErrorCode();

				LoadBuffer();
			}

			return RetVal;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	ssize_t FBufferedStream::Read(void* Buffer, size_t Size)
	{
		if (BaseStream) {
			size_t Remains = Size;
			size_t Reads = 0;

			ErrorCode = EStreamError::Success;
			while (Remains)
			{
				if (InternalBuffer.IsEndOfStream()) {
					if (bChanges && !bFlushAuto) {
						ErrorCode = EStreamError::BufferFull;
						break;
					}

					LoadBuffer();
				}

				if (!InternalBuffer.GetCapacity()) {
					break;
				}

				Reads = InternalBuffer.Read(Buffer, Remains);
				Remains -= Reads;
			}

			return Size - Remains;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	ssize_t FBufferedStream::Write(const void* Buffer, size_t Size)
	{
		if (BaseStream) {
			size_t Remains = Size;
			size_t Writes = 0;

			ErrorCode = EStreamError::NotSupported;

			if (BaseStream->HasCapability(EStreamCaps::Write)) {
				ErrorCode = EStreamError::Success;

				while (Remains)
				{
					Writes = InternalBuffer.Write(Buffer, Size);
					Remains -= Writes;

					if (Writes <= 0) {
						if (!bFlushAuto) {
							ErrorCode = EStreamError::BufferFull;
							break;
						}

						StoreBuffer();
					}

					else {
						bChanges = true;
					}
				}
			}

			return Size - Remains;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	bool FBufferedStream::Flush(bool bSync)
	{
		if (BaseStream) {
			StoreBuffer();
			ErrorCode = BaseStream->GetErrorCode();
			return true;
		}

		ErrorCode = EStreamError::NotOpened;
		return false;
	}

	bool FBufferedStream::Close()
	{
		if (BaseStream) {
			StoreBuffer();

			ErrorCode = BaseStream->GetErrorCode();
			BaseStream = nullptr;
			return true;
		}

		ErrorCode = EStreamError::NotOpened;
		return false;
	}

	void FBufferedStream::LoadBuffer()
	{
		if (BaseStream) {
			ssize_t Reads = 0;
			StoreBuffer();

			if (BaseStream->HasCapability(EStreamCaps::Read)) {
				Offset = BaseStream->Tell();
				InternalBuffer.Resize(Buffer);

				if (InternalBuffer.GetCapacity()) {
					Reads = BaseStream->Read(
						InternalBuffer.GetRaw(),
						InternalBuffer.GetCapacity());

					ErrorCode = BaseStream->GetErrorCode();
				}

				if (Reads <= 0) {
					Offset = 0;

					InternalBuffer.Close();
					InternalBuffer.SetMaxSize(Buffer);
				}

				else {
					InternalBuffer.Resize(Reads);
				}
			}
		}
	}

	void FBufferedStream::StoreBuffer()
	{
		if (BaseStream) {
			size_t InternalOffset = InternalBuffer.Tell();

			if (bChanges) {
				bChanges = false;

				if (BaseStream->HasCapability(EStreamCaps::Seek))
					BaseStream->Seek(Offset, EStreamSeek::Begin);

				if (BaseStream->HasCapability(EStreamCaps::Write)) {
					BaseStream->Write(
						InternalBuffer.GetRaw(),
						InternalBuffer.GetCapacity());

					ErrorCode = BaseStream->GetErrorCode();
				}
			}

			if (BaseStream->HasCapability(EStreamCaps::Seek))
				BaseStream->Seek(Offset + InternalOffset, EStreamSeek::Begin);

		}

		Offset = 0;

		InternalBuffer.Close();
		InternalBuffer.SetMaxSize(Buffer);
	}
}