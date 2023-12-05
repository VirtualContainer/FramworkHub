#ifndef RECYCLE_H
#define RECYCLE_H
#include<stdbool.h>

struct MemoryBlock;
struct MemoryNode;
struct MemoryPool
{
    struct MemoryBlock* block;
    struct MemoryNode* head;
    struct MemoryNode* current;
};

#define BufferQueueNum 8
struct BufferNode;
struct BufferQueue
{
   struct BufferNode* head;
   int num;
};


struct Recycler
{
    struct MemoryPool* m_pool;
    struct BufferQueue m_queue[BufferQueueNum];    
};

extern void InitRecycler(struct Recycler*);
extern void Allocate(struct Recycler*,void*,const unsigned int);
extern void Recycle(struct Recycler*,void*,const unsigned int);
extern void Release(struct Recycler*,void**,const unsigned int);
extern void Monitor(struct Recycler*);




#endif