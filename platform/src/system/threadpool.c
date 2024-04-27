#include "threadpool.h"
#include "message.h"

#ifdef DEBUG_MODE
    #include "init.h"
    #define threads_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"kernel","root",current_thread->m_attribute.u_feature.s_name,time_item,message,false);\
    }
    #define threads_debug(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Debug,"kernel","root",current_thread->m_attribute.u_feature.s_name,time_item,message,false);\
    }
#endif

static Thread_t root_thread = {.m_attribute={.u_feature={.s_id=0,.s_name="init"}},\
                              .m_status=INIT,\
                              .m_start_function=NULL,\
                              .m_task={.IsCycle=false,.function=NULL}};

static __thread Thread_t* current_thread = &root_thread;


typedef bind_func call_func;
static void* run(void* object)
{    
    if(object==NULL)
        return NULL;
    Thread_t* temp = (Thread_t*)object;
    call_func func;
    current_thread = temp;
    temp->m_status = RUNNING;
    func = temp->m_task.function;
    #ifdef DEBUG_MODE
        threads_debug("Thread Running");    
    #endif
    if(func)
        func(temp->m_message);
    else
        sleep(1);    
    temp->m_task.function = NULL;
    if(temp->m_task.IsCycle)
    {    
        temp->m_task.function = func;
        temp->m_status = READY;  
    }
    else
        temp->m_status = FREE;   
}

#include<string.h>
#include<assert.h>
void InitThread(Thread_t* thread,const pid_t id,const char* name,Task* task)
{
    thread->m_attribute.u_feature.s_id = id;
    assert(strlen(name)<=16);
    strcpy(thread->m_attribute.u_feature.s_name,name);
    thread->m_task.function = NULL;
    if(task)
    {
        thread->m_task.IsCycle = task->IsCycle;
        thread->m_task.function = task->function;
    }
    pthread_attr_init(&(thread->m_attribute.u_attr));
    thread->m_start_function = run;
    sem_init(&thread->m_semaphore,0,1);
}

void set_thread_attribute(Thread_t* thread,const size_t size,const int priority)
{
    size_t stack_size = size<<20;
    pthread_attr_setstacksize(&(thread->m_attribute.u_attr),stack_size);
    if(priority)
    {
        struct sched_param parameter;
        parameter.sched_priority = priority;
        pthread_attr_setschedparam(&(thread->m_attribute.u_attr),&parameter);
    }

}

void CreateThread(Thread_t* thread)
{
    //sem_wait(&(thread->m_semaphore));
    thread->m_status = INIT;
    //int status_code = pthread_create(&(thread->m_body),&(thread->m_attribute.u_attr),thread->m_start_function,thread);
    int status_code = pthread_create(&(thread->m_body),NULL,thread->m_start_function,thread);/*线程安全执行*/
    if(status_code)
    {
        thread->m_status = TERMINATE;
        #ifdef DEBUG_MODE
        threads_exception("Create Failed");    
        #endif
    }
    else
    {
        thread->m_status = READY;
        #ifdef DEBUG_MODE
        threads_debug("Create Thread");    
        #endif
    }    
}

 void HandleThread(Thread_t* thread,struct Message* message)
{
    sem_wait(&thread->m_semaphore);
    struct Message* result = (thread->m_message);
    pthread_join(thread->m_body,(void**)&message);
    if(result)
        memcpy((void*)message,(const void*)result,sizeof(struct Message));
    sem_post(&thread->m_semaphore);
}

void ExitThread(Thread_t* thread)
{
    sem_wait(&thread->m_semaphore); 
    pthread_exit(&thread->m_message);
    sem_post(&thread->m_semaphore);
}

void DestoryThread(struct Thread** thread)
{
    sem_wait(&(*thread)->m_semaphore); 
    pthread_cancel((*thread)->m_body);
    (*thread)->m_status = TERMINATE;
    sem_post(&(*thread)->m_semaphore);
    free(*thread);
    *thread = NULL;
}
