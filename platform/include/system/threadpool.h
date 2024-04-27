#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<stdatomic.h>

#include<pthread.h>
#include<semaphore.h>


struct Message;

typedef struct Message Result;

typedef void*(*run_func)(void*);
typedef void(*bind_func)(Result*);/*可重入函数*/

#include<stdbool.h>

typedef struct Task
{
    bool IsCycle;
    bind_func function;
}Task;

struct Thread;
typedef void(*init_func)(struct Thread*,void*);/*设置线程的属性特征*/
typedef void(*create_func)(struct Thread*);
typedef void(*handle_func)(struct Thread*,struct Message*);
typedef void(*abort_func)(struct Thread*);
typedef void(*exit_func)(struct Thread*);
/*线程实体*/
typedef struct Thread
{
    struct
    {
        struct
        {
            pid_t s_id;
            char s_name[16];
            size_t s_stack_size;
            struct sched_param s_scheduled_parameter;
        }u_feature;
        pthread_attr_t u_attr;
    }m_attribute;
    pthread_t  m_body;
    enum
    {
        INIT = 0,
        READY,
        RUNNING,
        BLOCKED,
        FREE,
        TERMINATE
    } m_status;
    struct Message* m_message;
    sem_t m_semaphore;
    Task m_task;
    run_func m_start_function;
#ifdef USE_THREAD_HANDLE    
    create_func m_create_handle;
    handle_func m_handle_handle;
    abort_func m_abort_handle;
    exit_func m_exit_handle;
#endif    
}Thread_t;

extern void InitThread(Thread_t* thread,const pid_t id,const char* name,Task* task);
extern void CreateThread(Thread_t* thread);
extern void HandleThread(Thread_t* thread,struct Message* message);
extern void ExitThread(Thread_t* thread);
#endif