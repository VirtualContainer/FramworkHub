#ifndef MESSAGE_H
#define MESSAGE_H
#include "lock.h"

typedef struct Message
{
    union
    {
        long long u_value;
        void* u_handle;
        void* u_buffer;
    } packet;
}Message_t;

typedef struct MessageQueue
{
    unsigned size;
    unsigned in,out;
    unsigned count;
    void** buffer;
    Sem_t  push_sync;
    Sem_t  pop_sync;
    Locker_t visit_lock;  
}MessageQueue_t;

extern void InitMQ(MessageQueue_t* queue,void** buffer_area,const unsigned size);
extern int PushMessage(MessageQueue_t* queue,Message_t* message,const unsigned ms);
extern int PopMessage(MessageQueue_t* queue,Message_t* message,const unsigned ms);


#endif