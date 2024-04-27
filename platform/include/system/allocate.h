#ifndef ALLOCATE_H
#define ALLOCATE_H
#include<stdbool.h>

#define PageSize (4*1024)
#define Align 8
#define MemoryPoolSize (4*1024)

/*内存分配器的实现，并且使用了无锁队列来保证在并发环境下的安全性*/
#ifdef Volatile
#undef Volatile
#endif
#ifdef CONCURRENCY_MODE
    #include "lock.h"
    #define Volatile volatile
#else
    #define Volatile
#endif

struct MemoryBlock;
struct BlockNode;
struct MemoryPool
{
    struct MemoryBlock* Volatile block;
    struct BlockNode* Volatile head;
    struct BlockNode* Volatile current;
};

#define BufferQueueNum 8
#define UnitNodeSize 32
struct ElementNode;
struct BufferQueue
{
   Volatile struct ElementNode* Volatile head;
   Volatile int num;
};


typedef struct Recycler
{
    Volatile struct MemoryPool* Volatile m_pool;/*用来分配256B到4KB之间大小的内存*/
    struct BufferQueue m_queue[BufferQueueNum];/*用来分配不大于256B的内存*/    
}Recycler_t;
typedef Recycler_t Allocater_t;

extern void InitAllocater(struct Recycler*);
extern void Allocate(struct Recycler*,void**,const unsigned int);
extern void Recycle(struct Recycler*,void*,const unsigned int);
extern void Release(struct Recycler*,void**,const unsigned int);
#ifdef DEBUG_MODE
    extern void Monitor(struct Recycler*);
#endif
extern void FreeAllocater(struct Recycler*);
#define InitRecycler InitAllocater
extern void CloseAllocater();

#endif