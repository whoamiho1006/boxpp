#pragma once

#if defined (IPC_IMPL_INCLUDES) && IPC_IMPL_INCLUDES
namespace boxpp {
	namespace ipc_internals {
#if PLATFORM_UNIX_KIND
		FASTINLINE bool IPC_EnsurePipeDirectory() {
			struct stat info;

			if (stat(BOXPP_IPC_DIRECTORY, &info)) {
				if (errno == ENOENT) {
					return !mkdir(BOXPP_IPC_DIRECTORY, 0755);
				}

				return false;
			}

			else if (!(info.st_mode & S_IFDIR)) {
				if (!unlink(BOXPP_IPC_DIRECTORY)) {
					return !mkdir(BOXPP_IPC_DIRECTORY, 0755);
				}

				return false;
			}

			return true;
		}
#endif
	}
}
#endif