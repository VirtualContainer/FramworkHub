#ifndef LOCK_H
#define LOCK_H

#define MB __sync_synchronize
#define LMB() asm volatile("":::"memory")
#define SMB() asm volatile("":::"memory")

#define AtomicXCHG __sync_lock_test_and_set
#define ATOMIC_ADD __sync_add_and_fetch
#define ATOMIC_CAS __sync_bool_compare_and_swap

#include <pthread.h>
#include <semaphore.h>

struct Sem
{
    int count;
    pthread_cond_t condition;
    pthread_mutex_t mutex;
};
typedef struct Sem* Sem_t;

extern Sem_t SemInit(const int count);
extern void SemNotify(Sem_t sem);
extern int SemWait(Sem_t sem,const unsigned ms);
extern void SemFree(Sem_t sem);

typedef pthread_mutex_t Locker_t;
#define Locker_init pthread_mutex_init
#define Locker_lock pthread_mutex_lock
#define Locker_unlock pthread_mutex_unlock
#define Locker_destroy pthread_mutex_destroy

#define GUARD_LOCK() {\
    unsigned char semphore = 1;\
    semphore--;\
    const int __lock_count__ __attribute__((unused)) = 1;
#define GUARD_UNLOCK() semphore++;\
}

#endif