#ifndef PTHREADRTNS_H
#define PTHREADRTNS_H

#include <pthread.h>

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

void PTHREAD_CREATE(pthread_t *thread, const pthread_attr_t *attr, void *(*startRoutine)(void *), void *arg);
void PTHREAD_MUTEX_INIT(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
void PTHREAD_MUTEX_LOCK(pthread_mutex_t *mutex);
void PTHREAD_MUTEX_UNLOCK(pthread_mutex_t *mutex);
void PTHREAD_MUTEX_DESTROY(pthread_mutex_t *mutex);
void PTHREAD_COND_INIT(pthread_cond_t *cond, pthread_condattr_t *attr);
void PTHREAD_COND_WAIT(pthread_cond_t *cond, pthread_mutex_t *mutex);
void PTHREAD_COND_SIGNAL(pthread_cond_t *cond);
void PTHREAD_BARRIER_INIT(pthread_barrier_t *barrier, pthread_barrierattr_t *attr, unsigned count);

#ifdef __cplusplus
}
#endif

#endif // PTHREADRTNS_H