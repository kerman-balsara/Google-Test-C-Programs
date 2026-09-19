#include <pthread.h>

#include <errcodes.h>

#include <pthreadrtns.h>
#include <sysexit.h>

void PTHREAD_CREATE(pthread_t *thread, const pthread_attr_t *attr, void *(*startRoutine)(void *), void *arg)
{
    int ret;
    ret = pthread_create(thread, attr, startRoutine, arg);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_CREATE);
}

void PTHREAD_MUTEX_INIT(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
    int ret;
    ret = pthread_mutex_init(mutex, attr);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_MUTEX_INIT);
}

void PTHREAD_MUTEX_LOCK(pthread_mutex_t *mutex)
{
    int ret;
    ret = pthread_mutex_lock(mutex);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_MUTEX_LOCK);
}

void PTHREAD_MUTEX_UNLOCK(pthread_mutex_t *mutex)
{
    int ret;
    ret = pthread_mutex_unlock(mutex);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_MUTEX_UNLOCK);
}

void PTHREAD_MUTEX_DESTROY(pthread_mutex_t *mutex)
{
    int ret;
    ret = pthread_mutex_destroy(mutex);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_MUTEX_DESTROY);
}

void PTHREAD_COND_INIT(pthread_cond_t *cond, pthread_condattr_t *attr)
{
    int ret;
    ret = pthread_cond_init(cond, attr);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_COND_INIT);
}

void PTHREAD_COND_WAIT(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    int ret;
    ret = pthread_cond_wait(cond, mutex);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_COND_WAIT);
}

void PTHREAD_COND_SIGNAL(pthread_cond_t *cond)
{
    int ret;
    ret = pthread_cond_signal(cond);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_COND_SIGNAL);
}

void PTHREAD_BARRIER_INIT(pthread_barrier_t *barrier, pthread_barrierattr_t *attr, unsigned count)
{
    int ret;
    ret = pthread_barrier_init(barrier, attr, count);
    if (ret)
        SysExit(__func__, ret, SYSCALL_PT_BARRIER_INIT);
}
