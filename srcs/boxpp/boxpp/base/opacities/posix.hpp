#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

/*
	Auto-completion support for pthread_* series.
*/

#if PLATFORM_POSIX
#	if !BOX_NOT_COMPILED
#		include <pthread.h>
#		include <signal.h>
#		include <dlfcn.h>
#		include <unistd.h>
#	else

typedef void* pthread_t;
struct pthread_attr_t { boxpp::s32 this_is_only_for_IDE; };
struct pthread_mutex_t { boxpp::s32 this_is_only_for_IDE; };
struct pthread_cond_t { boxpp::s32 this_is_only_for_IDE; };
struct pthread_condattr_t { boxpp::s32 this_is_only_for_IDE; };
struct pthread_mutexattr_t { boxpp::s32 this_is_only_for_IDE; };
struct pthread_key_t { boxpp::s32 this_is_only_for_IDE; };

#ifndef __TIMESPEC__
struct timespec { boxpp::s32 tv_sec; boxpp::s32 tv_nsec; boxpp::s32 this_is_only_for_IDE; };
#endif

boxpp::s32 pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);
boxpp::s32 pthread_mutex_lock(pthread_mutex_t *mutex);
boxpp::s32 pthread_mutex_trylock(pthread_mutex_t *mutex);
boxpp::s32 pthread_mutex_timedlock(pthread_mutex_t* mutex, const timespec* abs_timeout);
boxpp::s32 pthread_mutex_unlock(pthread_mutex_t *mutex);
boxpp::s32 pthread_mutex_destroy(pthread_mutex_t *mutex);

boxpp::s32 pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
boxpp::s32 pthread_cond_signal(pthread_cond_t *cond);
boxpp::s32 pthread_cond_broadcast(pthread_cond_t *cond);
boxpp::s32 pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
boxpp::s32 pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
boxpp::s32 pthread_cond_destroy(pthread_cond_t *cond);

boxpp::s32 pthread_create(pthread_t* thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
boxpp::s32 pthread_join(pthread_t thread, void **retval);
pthread_t pthread_self(void);
boxpp::s32 pthread_yield(void);

boxpp::s32 pthread_key_create(pthread_key_t *key, void(*destructor)(void*));
boxpp::s32 pthread_key_delete(pthread_key_t key);
boxpp::s32 pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);


void sleep(boxpp::u32 useconds);
void usleep(boxpp::u32 useconds);

struct sig_t
{
	boxpp::s32 this_is_only_for_IDE;
};

boxpp::s32 raise(sig_t sig);

constexpr sig_t SIGABRT = { 1 };
constexpr sig_t SIGFPE = { 2 };
constexpr sig_t SIGILL = { 3 };
constexpr sig_t SIGINT = { 4 };
constexpr sig_t SIGSEGV = { 5 };
constexpr sig_t SIGTERM = { 6 };

FASTINLINE void __builtin_trap() { }

void *dlopen(const char *filename, int flag);
char *dlerror(void);
void *dlsym(void *handle, const char *symbol);
int dlclose(void *handle);

/* Below error codes are dummy. (just defined) */

//#define EADDRINUSE	1
//#define EADDRNOTAVAIL 2
//#define EAGAIN 3
//#define EWOULDBLOCK 4
//#define EALREADY 5

//#define EBADF 6
#define EBADFD 7

//#define EBUSY 8
//#define ECANCELED 9

//#define ECONNABORTED 10
//#define ECONNREFUSED 11

//#define ECONNRESET 12

//#define EDESTADDRREQ 13
//#define EFAULT 14

//#define EHOSTDOWN 15
//#define EHOSTUNREACH 16

//#define EINPROGRESS 17
//#define EINTR 18

//#define EINVAL 19
//#define EISCONN 20

//#define EMFILE 21

//#define ENETDOWN 22
//#define ENETRESET 23
//#define ENETUNREACH 24

//#define ENFILE 25
//#define ENOBUFS 26
//#define ENODEV 27
//#define ENOMEM 28

//#define ENOPROTOOPT 29
//#define ENOTCONN 30

//#define ENOTSOCK 31
//#define ENOTSUP 32
//#define EOPNOTSUPP 33

//#define EPFNOSUPPORT 34
//#define EPROTONOSUPPORT 35
//#define EPROTOTYPE 36

//#define ESHUTDOWN 37
//#define ESOCKTNOSUPPORT 38

//#define ETIMEDOUT 39

//#define ETOOMANYREFS 40
//#define EUNATCH 41

//#define EAFNOSUPPORT 42
//#define ENOENT 43

FASTINLINE boxpp::s32 fcntl(boxpp::s32 s, boxpp::s32 m, boxpp::s32 r) { return -1; }

#define F_GETFL 1
#define F_SETFL 2

#define O_NONBLOCK 3

#	endif
#endif