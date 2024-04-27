#include "allocate.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#ifdef DEBUG_MODE
#include "init.h"
    #define allocate_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"kernel","init","init",time_item,message,false);\
    }
    #define allocate_debug(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Debug,"kernel","init","init",time_item,message,false);\
    }

#endif

/*判断是否相同内存地址*/
#define IsSameAddress(addr1,addr2)\
(\
{\
    uintptr_t _addr1 = (uintptr_t)addr1;\
    uintptr_t _addr2 = (uintptr_t)addr2;\
    _addr1==_addr2;\
}\
)

/*判断是否在一段内存空间内部*/
#define IsInerAddressZone(addr,lowest,highest)\
(\
{\
    uintptr_t _addr = (uintptr_t)addr;\
    uintptr_t _lowest = (uintptr_t)lowest;\
    uintptr_t _highest = (uintptr_t)highest;\
    _addr>=_lowest && _addr<=_highest;\
}\
)

/*向上扩展size为unit的整数倍大小*/
static inline unsigned int roundup(const unsigned int size,const unsigned int unit)
{   
    return (size+unit-1)&~(unit-1);
}


struct BlockNode
{
    struct BlockNode* Volatile next;
    unsigned char* Volatile last;//已分配的结尾地址
    unsigned char* Volatile end;//块的结尾地址
    Volatile int quote;//引用次数
    Volatile int failed;//失效次数    
};

/*可获取的最大空闲内存大小*/
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
    struct MemoryBlock* Volatile next;
    Volatile unsigned size;
    Volatile bool used;
    void* Volatile memory;
};

/*
                                last                                       end
    -------------------------------------------------------------------------
    |   pool   |    pool->head   |                     PageSize             |
    -------------------------------------------------------------------------
    这个PageSize可以用来分配MemoryBlock的结构表（分配大小大于4KB时），也可以分配目标对象内存（分配大小小于4KB）                                  
*/
static struct MemoryPool* CreateMemoryPool(const unsigned int size)
{
    Volatile struct MemoryPool* Volatile pool; 
    pool = aligned_alloc(Align,size);
    if(!pool)
    {
        #ifdef DEBUG_MODE
            allocate_exception("Allocate Failed");
        #endif
        return NULL;
    }
    pool->block = NULL;
    pool->current = pool->head = (struct BlockNode*)((unsigned char*) pool+sizeof(struct MemoryPool));
    pool->head->next = NULL;
    pool->head->last = (unsigned char*) pool+sizeof(struct MemoryPool)+sizeof(struct BlockNode);
    pool->head->end = (unsigned char*) (pool->head->last+PageSize);
    pool->head->failed = 0;
    pool->head->quote = 0;
    #ifdef DEBUG_MODE
        allocate_debug("MemoryPool Load");
    #endif
    return pool;
}

static void* MemoryPoolAllocate(struct MemoryPool* pool,const unsigned int size)
{
    assert(size>128);
    Volatile void* Volatile allocate;
    if(size<MemoryPoolSize)
    {    
        if(pool->current->failed>MaxFailedTimes)
        {
            if(pool->current->next==NULL)
            {
                pool->current->next = (struct BlockNode*) aligned_alloc(Align,PageSize+sizeof(struct BlockNode));
                struct BlockNode* next = pool->current->next;
                next->next = NULL;
                next->quote = 0;
                next->failed = 0;
                next->last = (unsigned char*) next+sizeof(struct BlockNode);
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
            if(!(*current_block)->used&&(*current_block)->size>=size)
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
            pool->current->failed = 0;
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
    struct BlockNode* node_head = pool->head;
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
                node_head->last = (unsigned char*) (node_head+sizeof(struct BlockNode));
            }
            break;    
        }
        node_head = node_head->next;    
    } 
}

static void MemoryPoolRelease(struct MemoryPool* pool,void* memory,const unsigned int size)
{
    if(!memory)
        return;
    assert(size>PageSize);
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
            struct BlockNode* node_head = pool->head;
            while(node_head)
            {
                if(IsInerAddressZone(*ptr,node_head,node_head->last))
                {    
                    node_head->quote--;
                    if(node_head->quote==0)
                    {    
                        pool->current = node_head;
                        node_head->failed = 0;
                        node_head->last = (unsigned char*) (node_head+sizeof(struct BlockNode));
                    }
                    break;
                }    
                node_head = node_head->next;    
            }
            memset(*ptr,0,sizeof(struct MemoryBlock));
        }
        ptr = &((*ptr)->next);
    }
    
}


/*分配小块内存*/
struct ElementNode
{
    bool used;
    struct ElementNode* Volatile next;
    Volatile unsigned size;
    char data[0];
};


static inline unsigned char GetQueueIndex(const unsigned int size)
{
    unsigned int unit_size = UnitNodeSize;
    return size/unit_size;
}

#define InitElementNode(node,allocate_size)\
{\
    node->used = false;\
    node->next = NULL;\
    node->size = allocate_size;\
}

static void* BufferQueueAllocate(struct BufferQueue* queue,const unsigned int size)
{
    const unsigned int allocate_size = size;
    struct ElementNode* ptr = queue->head;
    if(queue->num==0)
    {
        if(queue->head)
            InitElementNode(queue->head,allocate_size);
        queue->head = (struct ElementNode*)malloc(sizeof(struct ElementNode)+allocate_size);
        queue->head->used = true;
        queue->head->next = NULL;
        queue->num++;
        return queue->head->data;
    }
    struct ElementNode* last = ptr;
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
    ptr = (struct ElementNode*)malloc(sizeof(struct ElementNode)+allocate_size);
    InitElementNode(ptr,size);
    last->next = ptr;
    ptr->used = true;
    ptr->size = allocate_size;
    return ptr->data;
}

static void BufferQueueRecycle(struct BufferQueue* queue,void* object,const unsigned int size)
{
    struct ElementNode* ptr = queue->head;
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
    if(!(*object))
        return;
    struct ElementNode* head = queue->head;
    struct ElementNode* ptr = head;
    struct ElementNode* next;
    while(ptr)
    {
        if(IsSameAddress(ptr->data,*object))
        {
            next = ptr->next;
            queue->num--;
            free(*object);
            *object = NULL;            
            return;
        }
        ptr = ptr->next;
    }
}


void InitAllocater(struct Recycler* recycler)
{
    recycler->m_pool = NULL;
    unsigned char index;
    for(index=0;index<BufferQueueNum;index++)
    {
        recycler->m_queue[index].num = 0;
        recycler->m_queue[index].head = NULL;
    }
    #ifdef DEBUG_MODE
        allocate_debug("Init Allocater");
    #endif
    return;
}

void Allocate(struct Recycler* recycler,void** object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        *object = BufferQueueAllocate(&(recycler->m_queue[index]),size);            
        if(*object)
        {
            #ifdef DEBUG_MODE
                allocate_debug("Memory Allocate");   
            #endif
        }
        return;
    }
    unsigned int pool_size = (unsigned int) sizeof(struct MemoryPool)+sizeof(struct BlockNode)+PageSize;
    if(recycler->m_pool==NULL)
        recycler->m_pool = CreateMemoryPool(pool_size);
    unsigned int allocate_size = roundup(size,256);    
    *object = MemoryPoolAllocate(recycler->m_pool,allocate_size);        
}

void Recycle(struct Recycler* recycler,void* object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        BufferQueueRecycle(&(recycler->m_queue[index]),object,size);
        #ifdef DEBUG_MODE
            allocate_debug("Memory Recycle");
        #endif
        return;
    }
    unsigned int recycle_size = roundup(size,256);
    MemoryPoolRecycle(recycler->m_pool,object,recycle_size);
    #ifdef DEBUG_MODE
        allocate_debug("Memory Recycle");
    #endif
}

void Release(struct Recycler* recycler,void** object,const unsigned int size)
{
    if(size<=256)
    {
        unsigned char index = GetQueueIndex(size);
        BufferQueueRelease(&(recycler->m_queue[index]),object,size);
        *object = NULL;
        #ifdef DEBUG_MODE
            allocate_debug("Memory Release");
        #endif
        return;
    }
    if(size<PageSize)
    {
        unsigned int release_size = roundup(size,256);
        MemoryPoolRecycle(recycler->m_pool,object,release_size);
        #ifdef DEBUG_MODE
            allocate_debug("Memory Release");
        #endif
        return;
    }        
    MemoryPoolRelease(recycler->m_pool,*object,size);
    *object = NULL;
    #ifdef DEBUG_MODE
        allocate_debug("Memory Release");
    #endif
}

#ifdef DEBUG_MODE
void Monitor(struct Recycler* recycler)
{

}
#endif

void CloseAllocater(struct Recycler* recycler)
{
    int index;
    struct ElementNode *queue_head,*temp;
    for(index=0;index<BufferQueueNum;index++)
    {
        queue_head = recycler->m_queue[index].head;
        while(queue_head)
        {
            temp = queue_head;
            queue_head = queue_head->next;
            free(temp);            
        }
    }
    if(recycler->m_pool)
    {
        struct MemoryBlock* block_head = recycler->m_pool->block;
        while(block_head)
        {
            if(block_head->memory)
            {
                free(block_head->memory);
                block_head->memory = NULL;
            }
            block_head = block_head->next;
        }
        struct BlockNode* node_head = recycler->m_pool->head->next;
        struct BlockNode* current;
        while(node_head)
        {
            current = node_head;
            node_head = node_head->next;
            free(current);       
        }
        free(recycler->m_pool);
        recycler->m_pool = NULL;
    }
    #ifdef DEBUG_MODE
        allocate_debug("Close Allocater");
    #endif
}