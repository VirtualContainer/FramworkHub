#include"link.h"
#include<stdlib.h>

void InitLinklist(linklist list){
     list->s_num=0;
     list->s_headnode=(linklist_node*)malloc(sizeof(linklist_node));
     list->s_headnode->next=list->s_headnode;
     list->s_headnode->prev=list->s_headnode;
}

void AddLinklistnodeTail(linklist list,linklist_node* node){
     linklist_node* ptr=list->s_headnode;  
     while(ptr->next!=list->s_headnode){
        ptr=ptr->next;
     }
     node->prev=ptr;
     node->next=ptr->next;
     ptr->next=node;
     list->s_headnode->prev=node;
     list->s_num++;
}

void AddLinklistnodeIndex(linklist list,linklist_node* node,unsigned int index){
     assert(index <= (list->s_num));
     unsigned int counts=0;
     linklist_node* ptr=list->s_headnode;
     while(counts<index){
        ptr=ptr->next;
        counts++;
     }
     linklist_node* temp=ptr->next;
     ptr->next=node;
     node->prev=ptr;     
     node->next=ptr->next;
     temp->prev=node;
     list->s_num++;
}

void DeleteLinklistnode(linklist list,linklist_node* node)
{
     if(!list->s_num)
        return;
     linklist_node* ptr=list->s_headnode->next;
     while(ptr!=list->s_headnode){
          if(ptr==node){
               linklist_node* next=ptr->next;
               ptr->next=NULL;
               ptr->prev->next=next;
               next->prev=ptr->prev;
               ptr->prev=NULL;
               list->s_num--;
               return;
          }
          ptr=ptr->next;
     }          
}

void SearchLinklistnode(linklist list,linklist_node** node,const int key)
{
     *node = NULL;
     if(!list->s_num)
          return;
     linklist_node* ptr=list->s_headnode->next;
     while(ptr!=list->s_headnode)
     {
          if(ptr->data==key)
          {
               node = &ptr;
               break;    
          }
          ptr=ptr->next;              
     }          
}
