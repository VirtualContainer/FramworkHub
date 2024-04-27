#include "block.h"
#include "link.h"
#include "lock.h"

typedef struct Block
{
    void* allocate_start;
    Linker_t free_list;
    Sem_t allocate_sync;
    Locker_t visit_lock;
}Block_t;

#ifdef DEBUG_MODE 
    #include "init.h"
    #define block_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"kernel","root","root",time_item,message,false);\
    }
#endif

#include <assert.h>
int BlockInit(struct Block* block,void* memory,const int size,const int num)
{
    assert((size_t)size>sizeof(Linker_t));
    block->allocate_start = memory;
    Locker_init(&block->visit_lock,NULL);
    uint8_t* buffer = (uint8_t*)memory;
    InitLinklist(&block->free_list);
    int count;
    for(count=0;count<num;count++)
    {
        Node_t* node = (Node_t*)buffer;
        buffer += size;
        AddLinklistnodeTail(&block->free_list,node);
    }
    block->allocate_sync = SemInit(num);
    if(!block->allocate_sync)
    {
        Locker_destroy(&block->visit_lock);
        #ifdef DEBUG_MODE
            block_exception("Init Failed");
        #endif
        return -1;
    }
    return 0;
}

void* BlockAlloc(struct Block* block,const unsigned ms)
{
    int failed;
    if(ms<=0)
    {
        if(block->free_list.s_num==0)
        {
            #ifdef DEBUG_MODE
                block_exception("Allocated Failed");
            #endif
            return NULL;
        }
    }
    else
    {
        failed = SemWait(block->allocate_sync,ms);
        if(failed)
            return NULL;
    }
    void* result = NULL;
    Locker_lock(&block->visit_lock);
    result = GetoutHeadnode(&block->free_list);
    Locker_unlock(&block->visit_lock);
    return result;
}

void BlockFree(struct Block* block,void* node)
{
    Locker_lock(&block->visit_lock);
    AddLinklistnodeTail(&block->free_list,node);
    Locker_unlock(&block->visit_lock);
    SemNotify(block->allocate_sync);
}

