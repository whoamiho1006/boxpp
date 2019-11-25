#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

namespace boxpp
{

	enum class ESeekOrigin
	{
		Begin = 0,
		Current = 1,
		End = 2
	};

	class IStream {
	public:
		virtual ~IStream() { }
		
	public:
		/* Determines this stream can provide read method or not. */
		virtual bool CanRead() const = 0;

		/* Determines this stream can provide write method or not. */
		virtual bool CanWrite() const = 0;

		/* Determines this stream can provide seek method or not. */
		virtual bool CanSeek() const = 0;

		/* Determines this stream reached end of stream or not. */
		virtual bool IsEndOfStream() const = 0;

		/* Seek stream. */
		virtual bool Seek(ssize_t Offset, ESeekOrigin Origin = ESeekOrigin::Begin) = 0;
		
		/* Read bytes from stream. */
		virtual ssize_t Read(void* Buffer, size_t Length) = 0;

		/* Write bytes into stream. */
		virtual ssize_t Write(const void* Buffer, size_t Length) = 0;

		/* Close this stream. */
		virtual bool Close() = 0;
	};

}