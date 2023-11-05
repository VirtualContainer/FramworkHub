#ifndef LINK_H
#define LINK_H
#include"base.h"

struct list_node{
   struct list_node* prev;  
   struct list_node* next;
};
typedef struct list_node linklist_node;
struct linklist_structure{
   unsigned int s_num; 
   linklist_node* s_headnode;
}; 
typedef struct linklist_structure* linklist;

extern void InitLinklist(linklist list);
extern void AddLinklistnodeTail(linklist list,linklist_node* node);
extern void AddLinklistnodeIndex(linklist list,linklist_node* node,unsigned int index);
extern void DeleteLinklistnode(linklist list,linklist_node* linklist_node);

#define EntryListContainer(container_type,member,node) CONTAINER_POINTER(container_type,member,node)


#endif