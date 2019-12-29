#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

namespace boxpp
{
	enum class EStreamCaps
	{
		Read = 0,
		Write,
		Seek
	};

	enum class EStreamSeek
	{
		Begin = 0,
		Current,
		End
	};

	class IStream
	{
	public:
		virtual ~IStream() { }

	public:
		virtual bool IsValid() const = 0;
		virtual bool IsLocked() const = 0;

	public:
		virtual bool HasCapability(EStreamCaps Capability) const = 0;

	public:
		virtual ssize_t Tell() const = 0;
		virtual ssize_t Seek(ssize_t Offset, EStreamSeek Origin) = 0;

		virtual ssize_t Read(void* Buffer, size_t Size) = 0;
		virtual ssize_t Write(const void* Buffer, size_t Size) = 0;

		virtual bool Flush(bool bSync = true) = 0;
		virtual bool Close() = 0;
	};

}