#ifndef LINK_H
#define LINK_H
#include"base.h"

struct list_node{
   struct list_node* prev;  
   struct list_node* next;
#ifdef DATA_CARRIED
   struct 
   {
      union
      {
         int u_value;
         void* u_handle;
         #ifndef MAXBUFFERSIZE
         #define MAXBUFFERSIZE 1024
         #endif
         char u_buffer[MAXBUFFERSIZE];
      }s_payload;
      unsigned s_size;
   }data;
#endif
   int key;
};
typedef struct list_node Node_t;

#ifdef DATA_CARRIED
#define InitNode(node)\
{\
   node.prev = NULL;\
   node.next = NULL;\
   node.key = -1;\
   node.data.s_size = 0;\
}
#else
#define InitNode(node)\
{\
   node.prev = NULL;\
   node.next = NULL;\
   node.key = -1;\
}
#endif

extern void MigrateNode(Node_t* src,Node_t* dest);

typedef struct linklist_s{
   unsigned int s_num; 
   Node_t* s_headnode;
   Node_t* s_tailnode;
}Linker_t; 
typedef struct linklist_s* linklist;

extern void InitLinklist(linklist list);
extern void AddLinklistnodeHead(linklist list,Node_t* node);
extern void AddLinklistnodeTail(linklist list,Node_t* node);
extern Node_t* GetoutHeadnode(linklist list);
extern void DeleteLinklistnode(linklist list,Node_t* node);
extern void SearchLinklistnode(linklist list,Node_t** node,const int key);


#define LinkedHost(container_type,member,node) HOST_POINTER(container_type,member,node)

#define ShowHostList(link,host_type)\
{\
   Node_t* ptr=link->s_headnode;\
   printf("Hosts address:\n");\
   while(ptr) \
   { \
      printf("%p  ",LinkedHost(host_type,linklist_node,ptr)); \
      ptr = ptr->next;\
   } \
   printf("\n");\
}

#endif