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
		: File(nullptr), bLocked(0)
	{
	}

	FFileStream::FFileStream(const ansi_t* Path, EFileMode Mode)
		: File(nullptr), bLocked(0)
	{
		static const ansi_t* NFM_Rw = "r+";
		static const ansi_t* NFM_Trunc = "w+";

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
	}

	FFileStream::FFileStream(const wide_t* Path, EFileMode Mode)
		: File(nullptr), bLocked(0)
	{
		static const wide_t* NFMW_Rw = L"r+";
		static const wide_t* NFMW_Trunc = L"w+";

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
	}

	FFileStream::~FFileStream()
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			fclose(FSTREAM_ToFilePtr(File));
			File = nullptr;
		}
	}
	
	ssize_t FFileStream::Tell() const
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			return ftell(FSTREAM_ToFilePtr(File));
		}

		return -1;
	}

	ssize_t FFileStream::Seek(ssize_t Offset, EStreamSeek Origin)
	{
		FAtomicScope Guard(Atomic);

		if (File) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#endif

			switch (Origin) {
			case EStreamSeek::Begin:
				fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_SET);
				break;

			case EStreamSeek::Current:
				fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_CUR);
				break;

			case EStreamSeek::End:
				fseek(FSTREAM_ToFilePtr(File), Offset, SEEK_END);
				break;
			}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

			return ftell(FSTREAM_ToFilePtr(File));
		}

		return -1;
	}

	ssize_t FFileStream::Read(void* Buffer, size_t Size)
	{
		ssize_t R = -1;
		
		if (File) {
			SetLocked();

			if (File) {
				R = fread(Buffer, 1, Size, FSTREAM_ToFilePtr(File));
			}

			SetUnlocked();
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
			}

			SetUnlocked();
		}

		return R;
	}

	bool FFileStream::Flush(bool bSync)
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			fflush(FSTREAM_ToFilePtr(File));
			return true;
		}

		return false;
	}

	bool FFileStream::Close()
	{
		FAtomicScope Guard(Atomic);

		if (File) {
			fflush(FSTREAM_ToFilePtr(File));
			fclose(FSTREAM_ToFilePtr(File));

			File = nullptr;
			bLocked = 0;
			return true;
		}

		return false;
	}
}