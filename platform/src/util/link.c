#include "link.h"
#include <stdlib.h>
#include <string.h>

void MigrateNode(Node_t* src,Node_t* dest)
{
     dest->prev = src->prev;
     dest->next = src->next;
     dest->key = src->key;
#ifdef DATA_CARRIED
     dest->data.s_size = src->data.s_size;
     memcpy((void*)&dest->data.s_payload,(const void*)&src->data.s_payload,sizeof(src->data.s_payload));
#endif     
     InitNode((*src));     
}

void InitLinklist(linklist list)
{
     list->s_num=0;
     list->s_headnode = NULL;
     list->s_tailnode = NULL;
}

void AddLinklistnodeHead(linklist list,Node_t* node)
{
     node->prev = NULL;
     node->next = list->s_headnode;
     if(list->s_num==0)
     {
          list->s_headnode = node;
          list->s_tailnode = node;
     }
     else
     {
          list->s_headnode->prev = node;
          list->s_headnode = node;
     }
     list->s_num++;
}

void AddLinklistnodeTail(linklist list,Node_t* node)
{
     node->prev = list->s_tailnode;
     node->next = NULL;
     if(list->s_num==0)
     {
          list->s_headnode = node;
          list->s_tailnode = node;
     }
     else
     {
          list->s_tailnode->next = node;
          list->s_tailnode = node;
     }
     list->s_num++;
}

void DeleteLinklistnode(linklist list,Node_t* node)
{
     if(!list->s_num)
        return;
     Node_t* ptr=list->s_headnode;
     while(ptr && ptr!=node)
     {
          ptr = ptr->next;
     }
     if(!ptr)
          return;
     Node_t* next = ptr->next;
     Node_t* prev = ptr->prev;
     if(next)
          next->prev = prev;
     else
          list->s_tailnode = prev;
     if(prev)
          prev->next = next;
     else
          list->s_headnode = next;
     list->s_num--;
}

void SearchLinklistnode(linklist list,Node_t** node,const int key)
{
     *node = NULL;
     if(!list->s_num)
          return;
     Node_t* ptr=list->s_headnode;
     while(ptr)
     {
          if(ptr->key==key)
          {
               *node = ptr;
               break;    
          }
          ptr=ptr->next;              
     }          
}

Node_t* GetoutHeadnode(linklist list)
{
     if(!list->s_num)
          return NULL;
     Node_t* head = list->s_headnode;
     Node_t* next = head->next;
     if(next)
     {
          next->prev = NULL;
          list->s_headnode = next;
     }
     else
     {
          list->s_headnode = NULL;
          list->s_tailnode = NULL;          
     }
     list->s_num--;
     return head;
}
