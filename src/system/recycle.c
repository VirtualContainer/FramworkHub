#include"recycle.h"
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<assert.h>

#define PageSize (4*1024)
#define Align 8
#define MemoryPoolSize (4*1024)

#define IsSameAddress(addr1,addr2)\
(\
{\
    uintptr_t _addr1 = (uintptr_t)addr1;\
    uintptr_t _addr2 = (uintptr_t)addr2;\
    _addr1==_addr2;\
}\
)

#define IsInerAddressZone(addr,lowest,highest)\
(\
{\
    uintptr_t _addr = (uintptr_t)addr;\
    uintptr_t _lowest = (uintptr_t)lowest;\
    uintptr_t _highest = (uintptr_t)highest;\
    _addr>=_lowest && _addr<=_highest;\
}\
)

static inline unsigned int roundup(const unsigned int size,const unsigned int unit)
{   
    return (size+unit-1)&~(unit-1);
}

/*分配大块内存*/
struct MemoryNode
{
    struct MemoryNode* next;
    unsigned char* last;//已分配的结尾地址
    unsigned char* end;//块的结尾地址
    int quote;//引用次数
    int failed;//失效次数    
};

#define ResidueSize(node)\
(\
{\
    unsigned int size = (unsigned int) (node->end-node->last);\
    size;\
}\
)

#define MaxFailedTimes 4

struct MemoryBlock
{
    struct MemoryBlock* next;
    unsigned int size;
    bool used;
    void* memory;
};



static struct MemoryPool* CreateMemoryPool(const unsigned int size)
{
    struct MemoryPool* pool; 
    pool = aligned_alloc(Align,size);
    if(!pool)
        return NULL;
    pool->block = NULL;
    pool->current = pool->head = (struct MemoryNode*)((unsigned char*) pool+sizeof(struct MemoryPool));
    pool->head->next = NULL;
    pool->head->last = (unsigned char*) pool+sizeof(struct MemoryPool)+sizeof(struct MemoryNode);
    pool->head->end = (unsigned char*) (pool->head->last+PageSize);
    pool->head->failed = 0;
    pool->head->quote = 0;
    return pool;
}

static void* MemoryPoolAllocate(struct MemoryPool* pool,const unsigned int size)
{
    assert(size>128);
    void* allocate;
    if(size<MemoryPoolSize)
    {    
        if(pool->current->failed>MaxFailedTimes)
        {
            if(pool->current->next==NULL)
            {
                pool->current->next = (struct MemoryNode*) aligned_alloc(Align,PageSize+sizeof(struct MemoryNode));
                struct MemoryNode* next = pool->current->next;
                next->next = NULL;
                next->quote = 0;
                next->failed = 0;
                next->last = (unsigned char*) next+sizeof(struct MemoryNode);
                next->end = (unsigned char*) next->last+PageSize;
            }
            pool->current = pool->current->next;
        }
        if(ResidueSize(pool->current)<size)
        {
            pool->current->failed++;
            return NULL;
        }
        pool->current->failed = 0;
        pool->current->quote++;
        allocate = pool->current->last;
        pool->current->last = (unsigned char*) pool->current->last+size;
    }
    else
    {
        if(ResidueSize(pool->current)<sizeof(struct MemoryBlock))
        {
            pool->current->failed++;
            return NULL;
        }
        struct MemoryBlock** current_block = &pool->block;
        while(*current_block)
        {
            if((*current_block)->used==false&&(*current_block)->size>=size)
                break;
            current_block = &((*current_block)->next);
        }
        if((*current_block)==NULL)    
        {
            *current_block = (struct MemoryBlock*) (pool->current->last);    
            pool->current->last = (unsigned char*) pool->current->last+sizeof(struct MemoryBlock);
            (*current_block)->size = size;
            (*current_block)->next = NULL;
            (*current_block)->memory = aligned_alloc(Align,size);
            pool->current->quote++;
        }
        (*current_block)->used = true;
        allocate = (*current_block)->memory;
    }
    return allocate;    
}

static void MemoryPoolRecycle(struct MemoryPool* pool,void* memory,const unsigned int size)
{
    if(size>PageSize)
    {
        struct MemoryBlock* current_block = pool->block;
        while(current_block)
        {
            if(IsSameAddress(memory,current_block->memory))
            {
                memset(current_block->memory,0,current_block->size);
                current_block->used = false;
                break;
            }
            current_block = current_block->next;
        }
        return;
    }
    struct MemoryNode* node_head = pool->head;
    while(node_head)
    {
        if(IsInerAddressZone(memory,node_head,node_head->last))
        {
            memset(memory,0,size);
            node_head->quote--;
            if(node_head->quote==0)
            {    
                pool->current = node_head;
                node_head->failed = 0;
                node_head->last = (unsigned char*) (node_head+sizeof(struct MemoryNode));
            }
            break;    
        }
        node_head = node_head->next;    
    } 
}

static void MemoryPoolRelease(struct MemoryPool* pool,void* memory,const unsigned int size)
{
    if(size>PageSize)
    {
        struct MemoryBlock** ptr = &(pool->block);
        while(*ptr)
        {
            if(IsSameAddress((*ptr)->memory,memory))
            {
                free((*ptr)->memory);
                (*ptr)->memory = NULL;
                (*ptr)->size = 0;
                struct MemoryBlock* free_block = *ptr;
                *ptr = (*ptr)->next;
                struct MemoryNode* head = pool->head;
                while(head)
                {
                    if(IsInerAddressZone(*ptr,head,head->last))
                        head->quote--;    
                    head = head->next;    
                }
                memset(*ptr,0,sizeof(struct MemoryBlock));

            }
            ptr = &((*ptr)->next);
        }
    }


}

#define DefaultBufferNodeSize 32
/*分配小块内存*/
struct BufferNode
{
    bool used;
    struct BufferNode* next;
    struct BufferNode** pre;
    unsigned int size;
    char data[0];
};


static inline unsigned char GetQueueIndex(const unsigned int size)
{
    unsigned int unit_size = DefaultBufferNodeSize;
    return size/unit_size;
}

#define InitBufferNode(node,allocate_size)\
{\
    node->used = false;\
    node->next = NULL;\
    node->pre = NULL;\
    node->size = allocate_size;\
}

static void* BufferQueueAllocate(struct BufferQueue* queue,const unsigned int size)
{
    const unsigned int allocate_size = size;
    struct BufferNode* ptr = queue->head;
    if(queue->num==0)
    {
        InitBufferNode(queue->head,allocate_size);
        queue->head = (struct BufferNode*)malloc(sizeof(struct BufferNode)+allocate_size);
        queue->head->pre = &(queue->head);
        queue->head->used = true;
        queue->num++;
        return queue->head->data;
    }
    struct BufferNode* last;
    while(ptr)
    {
        if(ptr->used || ptr->size<size)
        {
            last = ptr;
            ptr = ptr->next;
            continue;
        }
        ptr->used = true;
        return ptr->data;
    }
    ptr = (struct BufferNode*)malloc(sizeof(struct BufferNode)+allocate_size);
    InitBufferNode(ptr,size);
    last->next = ptr;
    ptr->pre = &(last->next);
    ptr->used = true;
    ptr->size = allocate_size;
    return ptr->data;
}

static void BufferQueueRecycle(struct BufferQueue* queue,void* object,const unsigned int size)
{
    struct BufferNode* ptr = queue->head;
    while(ptr)
    {
        if(IsSameAddress(ptr->data,object))
        {
            memset(ptr->data,0,size);
            ptr->used = false;
            queue->num--; 
            return;   
        }
        ptr = ptr->next;
    }
}

static void BufferQueueRelease(struct BufferQueue* queue,void** object,const unsigned int size)
{
    struct BufferNode* head = queue->head;
    struct BufferNode* ptr = head;
    struct BufferNode* next;
    while(ptr)
    {
        if(IsSameAddress(ptr->data,*object))
        {
            next = ptr->next;
            *(ptr->pre) = next;
            if(next)
                next->pre = ptr->pre;
            queue->num--;
            free(*object);
            *object = NULL;            
            return;
        }
        ptr = ptr->next;
    }
}


void InitRecycler(struct Recycler* recycler)
{
    recycler->m_pool = NULL;
    unsigned char index;
    for(index=0;index<BufferQueueNum;index++)
    {
        recycler->m_queue[index].num = 0;
        recycler->m_queue[index].head = NULL;
    }
}

void Allocate(struct Recycler* recycler,void* object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        object = BufferQueueAllocate(&(recycler->m_queue[index]),size);            
        return;
    }
    unsigned int pool_size = (unsigned int) sizeof(struct MemoryPool)+sizeof(struct MemoryNode)+PageSize;
    if(recycler->m_pool==NULL)
        recycler->m_pool = CreateMemoryPool(pool_size);
    unsigned int allocate_size = roundup(size,256);    
    object = MemoryPoolAllocate(recycler->m_pool,allocate_size);        
}

void Recycle(struct Recycler* recycler,void* object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        BufferQueueRecycle(&(recycler->m_queue[index]),object,size);
        return;
    }
    unsigned int recycle_size = roundup(size,256);
    MemoryPoolRecycle(recycler->m_pool,object,recycle_size);
}

void Release(struct Recycler* recycler,void** object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        BufferQueueRelease(&(recycler->m_queue[index]),object,size);
        return;
    }
    MemoryPoolRelease(recycler->m_pool,*object,size);
    *object = NULL;
}

void Monitor(struct Recycler* recycler)
{

}