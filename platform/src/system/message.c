#include "message.h"

#ifdef DEBUG_MODE
    #include "init.h"
    #define messagequeue_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"kernel","root","root",time_item,message,false);\
    }
    #define messagequeue_debug(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_DEBUG,"kernel","root","root",time_item,message,false);\
    }
#endif

void InitMQ(MessageQueue_t* queue,void** buffer_area,const unsigned size)
{
    queue->size = size;
    queue->buffer = buffer_area;
    queue->in = 0;
    queue->out = 0;
    queue->count = 0;
    queue->push_sync = SemInit(size);
    queue->pop_sync = SemInit(0);
    Locker_init(&queue->visit_lock,NULL);
}

int PushMessage(MessageQueue_t* queue,Message_t* message,const unsigned ms)
{
    Locker_lock(&queue->visit_lock);
    if(ms<=0)
    {
        if(queue->count==queue->size)
            goto exception;
    }
    Locker_unlock(&queue->visit_lock);
    int failed = SemWait(queue->push_sync,ms);
    Locker_lock(&queue->visit_lock);
    if(failed || queue->count==queue->size)
        goto exception;
    queue->buffer[queue->in] = message->packet.u_buffer;
    queue->in = (queue->in+1)%queue->size;
    queue->count++;
    Locker_unlock(&queue->visit_lock);
    SemNotify(queue->pop_sync);
    return 0;
exception:
    Locker_unlock(&queue->visit_lock);
    #ifdef DEBUG_MODE
        messagequeue_exception("Push Failed");
    #endif
    return -1;
}

int PopMessage(MessageQueue_t* queue,Message_t* message,const unsigned ms)
{
    Locker_lock(&queue->visit_lock);
    if(ms<=0)
    {
        if(queue->count==0)
            goto exception;
    }
    Locker_unlock(&queue->visit_lock);
    int failed = SemWait(queue->pop_sync,ms);
    Locker_lock(&queue->visit_lock);
    if(failed || queue->count==0)
        goto exception;
    message->packet.u_buffer = queue->buffer[queue->out];
    queue->out = (queue->out+1)%queue->size;
    queue->count--;
    Locker_unlock(&queue->visit_lock);
    SemNotify(queue->push_sync);
    return 0;
exception:
    Locker_unlock(&queue->visit_lock);
    #ifdef DEBUG_MODE
        messagequeue_exception("Pop Failed");
    #endif
    return -1;

}