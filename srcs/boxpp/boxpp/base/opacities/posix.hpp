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
#		include <stdlib.h>
#		include <termios.h>
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

// -- unistd.h
int mkdir(const char*, boxpp::u32);
int unlink(const char*);
int close(int);

/* Below error codes are dummy. (just defined) */

#define EBADFD 7

FASTINLINE boxpp::s32 fcntl(boxpp::s32 s, boxpp::s32 m, boxpp::s32 r) { return -1; }

#define F_GETFL 1
#define F_SETFL 2

#define O_NONBLOCK 3

struct sockaddr_un { 
	boxpp::u32 sun_family; /* AF_UNIX */ 
	char sun_path[256]; /* pathname */ 
};

#define AF_UNIX		0xfff

boxpp::ssize_t read(int fd, void *buf, boxpp::size_t count);
boxpp::ssize_t write(int fd, const void *buf, boxpp::size_t count);

struct termios
{
	boxpp::u32 c_iflag;    /* input flags */
	boxpp::u32 c_oflag;    /* output flags */
	boxpp::u32 c_cflag;    /* control flags */
	boxpp::u32 c_lflag;    /* local flags */
	boxpp::u8  c_cc[1]; /* control chars */
	boxpp::u32 c_ispeed;   /* input speed */
	boxpp::u32 c_ospeed;   /* output speed */
};

int tcgetattr(int fd, struct termios*);
int tcsetattr(int fd,
	int optional_actions,
	const struct termios *termios_p);

#define ICANON	1
#define ECHO	2
#define TCSANOW 3
#	endif
#endif