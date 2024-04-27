#include "lock.h"
#include <stdlib.h>

#ifdef DEBUG_MODE
    #include "init.h"
    #define lock_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"kernel","root","root",time_item,message,false);\
    }
#endif

Sem_t SemInit(const int count)
{
    Sem_t sem = (Sem_t)malloc(sizeof(struct Sem));
    pthread_mutex_init(&sem->mutex,NULL);
    sem->count = count;
    pthread_cond_init(&sem->condition,NULL);
    return sem;
}

void SemNotify(Sem_t sem)
{
    pthread_mutex_lock(&sem->mutex);
    sem->count++;
    pthread_cond_signal(&sem->condition);
    pthread_mutex_unlock(&sem->mutex);
}

#include <time.h>
#include <errno.h>
int SemWait(Sem_t sem,const unsigned ms)
{
    pthread_mutex_lock(&sem->mutex);
    if(sem->count<=0)
    {
        int ret;
        if(ms>0)
        {
            struct timespec t;
            t.tv_nsec = (ms % 1000) * 1000000L;
            t.tv_sec = time(NULL) + ms / 1000;
            ret = pthread_cond_timedwait(&sem->condition, &sem->mutex, &t);
            if (ret == ETIMEDOUT) 
            {
                pthread_mutex_unlock(&sem->mutex);
                #ifdef DEBUG_MODE
                    lock_exception("Time Out");
                #endif
                return -1;
            }
        }
        else
        {
            ret = pthread_cond_wait(&sem->condition,&sem->mutex);
            if (ret<0) 
            {
                pthread_mutex_unlock(&sem->mutex);
                #ifdef DEBUG_MODE
                    lock_exception("Wait Failed");
                #endif
                return -1;
            }
        }
    }
    sem->count--;
    pthread_mutex_unlock(&sem->mutex);
    return 0;
}

void SemFree(Sem_t sem)
{
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->condition);    
}