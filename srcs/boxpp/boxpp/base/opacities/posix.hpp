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

#	endif
#endif