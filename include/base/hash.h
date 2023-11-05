#ifndef HASH_H
#define HASH_H
#include"base.h"
#include<stdlib.h>

struct list_recordingnode{
   struct list_recordingnode* next;
   struct list_recordingnode** prev_firstregion;
};
typedef struct list_recordingnode hashlist_node;

#define MAX_NODES 128

struct hashlist_head{
    hashlist_node* firstnode;
};

/*hashtable structure*/
typedef int (*HashFunction)(int);
struct hashlist_structure{
    struct hashlist_head hashtable[MAX_NODES];    
    unsigned int nodenum[MAX_NODES];
    HashFunction func;
};
typedef struct hashlist_structure* hashlist;



#define IsUnhashed(node)(\
    !(*node->prev_firstregion)\
)

static inline void InitHashTableNode(struct hashlist_head* head){
    assert(head);
    head->firstnode=NULL;
}

extern void InitHashlist(hashlist list);
extern void AddHashNode(hashlist list,const unsigned int key,hashlist_node* node);
extern void DeleteHashNode(hashlist list,const unsigned int key,hashlist_node* node);

#define IsUnconflictedHashNode(list,index)(\
{\
    struct hashlist_head* head=&(list->hashtable[index]);\
    head->firstnode && !(head->firstnode->next)\
}\
)

#define EntryHashnodeContainer(container_type,member,node) CONTAINER_POINTER(container_type,member,node) 

#endif