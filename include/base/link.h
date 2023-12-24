#ifndef LINK_H
#define LINK_H
#include"base.h"

struct list_node{
   struct list_node* prev;  
   struct list_node* next;
   int data;
};
typedef struct list_node linklist_node;
struct linklist_structure{
   unsigned int s_num; 
   linklist_node* s_headnode;//哨兵节点，不存储数据
}; 
typedef struct linklist_structure* linklist;
typedef struct linklist_structure linker;

extern void InitLinklist(linklist list);
extern void AddLinklistnodeTail(linklist list,linklist_node* node);
extern void AddLinklistnodeIndex(linklist list,linklist_node* node,unsigned int index);
extern void DeleteLinklistnode(linklist list,linklist_node* linklist_node);
extern void SearchLinklistnode(linklist list,linklist_node** node,const int key);


#define LinkedHost(container_type,member,node) HOST_POINTER(container_type,member,node)

#define ShowHostList(link,host_type)\
{\
   linklist_node* ptr=link->s_headnode->next;\
   while(ptr!=link->s_headnode) \
   { \
      printf("%p",LinkedHost(host_type,linklist_node,ptr)); \
      ptr=ptr->next;\
   } \
   printf("\n");\
}

#endif