#include "FileStream.hpp"
#include <boxpp/base/tpls/strings/String.hpp>
#include <stdio.h>

namespace boxpp {

	FASTINLINE FILE* FSTREAM_ToFilePtr(const void* fp) {
		return (FILE*)fp;
	}

#if PLATFORM_WINDOWS
	FASTINLINE FILE* FSTREAM_wfopen(const wide_t* Path, const wide_t* Mode) {
		return _wfopen(Path, Mode);
	}
#else
	FILE* FSTREAM_wfopen(const wide_t* Path, const wide_t* Mode) {
		using WideToAnsi = TStringConverter<ansi_t, wide_t>;
		static FAtomicBarrior __BARRIOR;

		static WideToAnsi CACHED_AnsiPath;
		static WideToAnsi CACHED_AnsiMode;

		FAtomicScope __GUARD(__BARRIOR);

		CACHED_AnsiPath.Reset(Path);
		CACHED_AnsiMode.Reset(Mode);

		if (CACHED_AnsiPath && CACHED_AnsiMode) {
			return fopen(CACHED_AnsiPath, CACHED_AnsiMode);
		}

		return nullptr;
	}
#endif

	FFileStream::FFileStream()
		: bEndOfStream(true), File(nullptr), bLocked(0),
		  ErrorCode(EStreamError::NotFound)
	{
	}

	FFileStream::FFileStream(const ansi_t* Path, EFileMode Mode)
		: bEndOfStream(false), File(nullptr), bLocked(0)
	{
		static const ansi_t* NFM_Rw = "r+";
		static const ansi_t* NFM_Trunc = "w+";

		ErrorCode = EStreamError::Success;

		switch (Mode) {
		case EFileMode::ReadOnly:
			File = fopen(Path, NFM_Rw);
			break;

		case EFileMode::ReadWrite:
			if (!(File = fopen(Path, NFM_Rw))) {
				File = fopen(Path, NFM_Trunc);
			}
			break;

		case EFileMode::AlwaysNew:
			File = fopen(Path, NFM_Trunc);
			break;
		}

		if (!File) {
			ErrorCode = ToStreamError(errno);
			bEndOfStream = true;
		}
	}

	FFileStream::FFileStream(const wide_t* Path, EFileMode Mode)
		: bEndOfStream(false), File(nullptr), bLocked(0)
	{
		static const wide_t* NFMW_Rw = L"r+";
		static const wide_t* NFMW_Trunc = L"w+";
		
		ErrorCode = EStreamError::Success;

		switch (Mode) {
		case EFileMode::ReadOnly:
			File = FSTREAM_wfopen(Path, NFMW_Rw);
			break;

		case EFileMode::ReadWrite:
			if (!(File = FSTREAM_wfopen(Path, NFMW_Rw))) {
				File = FSTREAM_wfopen(Path, NFMW_Trunc);
			}
			break;

		case EFileMode::AlwaysNew:
			File = FSTREAM_wfopen(Path, NFMW_Trunc);
			break;
		}

		if (!File) {
			ErrorCode = ToStreamError(errno);
			bEndOfStream = true;
		}
	}

	FFileStream::~FFileStream()
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			fclose(FSTREAM_ToFilePtr(File));

			bEndOfStream = true;
			File = nullptr;
		}
	}
	
	ssize_t FFileStream::Tell() const
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			ssize_t R = ftell(FSTREAM_ToFilePtr(File));

			ErrorCode = EStreamError::Success;
			if (R < 0) {
				ErrorCode = ToStreamError(errno);
			}

			return R;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	ssize_t FFileStream::Seek(ssize_t Offset, EStreamSeek Origin)
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			size_t Current = ftell(FSTREAM_ToFilePtr(File)), Temp = 0;
			EStreamError FinalError = EStreamError::Success;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			switch (Origin) {
			case EStreamSeek::Begin:
				if (fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_SET)) {
					FinalError = ToStreamError(errno);
				}
				break;

			case EStreamSeek::Current:
				if (fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_CUR)) {
					FinalError = ToStreamError(errno);
				}
				break;

			case EStreamSeek::End:
				if (fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_END)) {
					FinalError = ToStreamError(errno);
				}

				if (Offset >= 0) {
					bEndOfStream = true;
				}
				else {
					bEndOfStream = false;
				}
				break;
			}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			Temp = ftell(FSTREAM_ToFilePtr(File));
			if (Current > Temp)
				bEndOfStream = false;

			ErrorCode = FinalError;
			return Temp;
		}

		ErrorCode = EStreamError::NotOpened;
		return -1;
	}

	ssize_t FFileStream::Read(void* Buffer, size_t Size)
	{
		ssize_t R = -1;
		
		if (File) {
			SetLocked();

			if (File) {
				R = fread(Buffer, 1, Size, FSTREAM_ToFilePtr(File));

				if (R >= 0) {
					if (R < ssize_t(Size)) {
						bEndOfStream = true;
					}

					else {
						bEndOfStream = false;
					}
				}
				else {
					ErrorCode = ToStreamError(errno);
				}
			}

			SetUnlocked();
		}

		else {
			ErrorCode = EStreamError::NotOpened;
		}

		return R;
	}

	ssize_t FFileStream::Write(const void* Buffer, size_t Size)
	{
		ssize_t R = -1;

		if (File) {
			SetLocked();

			if (File) {
				R = fwrite(Buffer, 1, Size, FSTREAM_ToFilePtr(File));

				if (R >= 0) {
					if (R < ssize_t(Size))
						bEndOfStream = true;

					else bEndOfStream = false;
				}
				else {
					ErrorCode = ToStreamError(errno);
				}
			}

			SetUnlocked();
		}

		else {
			ErrorCode = EStreamError::NotOpened;
		}

		return R;
	}

	bool FFileStream::Flush(bool bSync)
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			size_t Cursor = ftell(FSTREAM_ToFilePtr(File)), Temp = 0;
			EStreamError FinalError = EStreamError::Success;

			if (fflush(FSTREAM_ToFilePtr(File))) {
				FinalError = ToStreamError(errno);
			}

			fseek(FSTREAM_ToFilePtr(File), 0, SEEK_END);
			Temp = ftell(FSTREAM_ToFilePtr(File));

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			fseek(FSTREAM_ToFilePtr(File), Cursor, SEEK_SET);

#ifdef _MSC_VER
#pragma warning(pop)
#endif
			bEndOfStream = Temp <= Cursor;
			ErrorCode = FinalError;
			return true;
		}

		ErrorCode = EStreamError::NotOpened;
		return false;
	}

	bool FFileStream::Close()
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			fflush(FSTREAM_ToFilePtr(File));
			fclose(FSTREAM_ToFilePtr(File));

			bEndOfStream = true;
			File = nullptr;
			bLocked = 0;

			ErrorCode = EStreamError::Success;
			return true;
		}

		ErrorCode = EStreamError::NotOpened;
		return false;
	}
}