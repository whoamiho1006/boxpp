#include "Stream.hpp"

#include <errno.h>

namespace boxpp {

	BOXPP EStreamError IStream::ToStreamError(s32 StdErr)
	{
		if (StdErr) {
			switch (StdErr) {
			case EINPROGRESS:
				return EStreamError::InProgress;

			case EINTR:
				return EStreamError::Interrupted;

			case ENAMETOOLONG:
				return EStreamError::NameTooLong;

			case EEXIST:
				return EStreamError::Exists;

			case EBUSY:
				return EStreamError::Busy;

			case EACCES:
			case EPERM:
				return EStreamError::NoPermission;

			case EMFILE:
			case EMLINK:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
				return EStreamError::NoResources;

			case ENOSPC:
				return EStreamError::NoSpace;

#if defined(ENOANO)
			case ENOANO:
#endif
			case ENODEV:
			case ENOENT:
				return EStreamError::NotFound;

			case EINVAL:
				return EStreamError::InvalidArgument;

			case ESPIPE:
				return EStreamError::InvalidSeek;

			case EOVERFLOW:
				return EStreamError::Overflow;

			case ELOOP:
				return EStreamError::Loop;

			case EIO:
				return EStreamError::IOError;

			case EFBIG:
				return EStreamError::TooBig;

			default:
				break;
			}

			return EStreamError::NotWrapped;
		}

		return EStreamError::Success;
	}
}