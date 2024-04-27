#include"hash.h"


void InitHashlist(hashlist list){
    if(!list)
        return;  
    for(int i=0;i<MAX_NODES;i++){
        InitHashTableNode(&(list->hashtable[i]));
        list->nodenum[i]=0;
    }    
}

/*insert hash node*/
static inline void InsertHashNode_Head(struct hashlist_head* head,hashlist_node* node){
    hashlist_node* first=head->firstnode;
    node->next=first;
    if(first)
        first->prev_firstregion=&(node->next);
    head->firstnode=node;
    node->prev_firstregion=&(head->firstnode);  
}

static inline void InsertHashNode_Next(hashlist_node* last,hashlist_node* node){
    assert(IsUnhashed(node));
    assert(last);
    hashlist_node* next=last->next;
    last->next=node;
    node->prev_firstregion=&(last->next);
    node->next=next;
    if(next)
       next->prev_firstregion=&(node->next);
}

static inline void InsertHashNode_Tail(struct hashlist_head* head,hashlist_node* node){
    hashlist_node* first=head->firstnode;
    if(!first){
        node->next=first;
        head->firstnode=node;
        node->prev_firstregion=&(head->firstnode);
        return;
    }
    hashlist_node* ptr=first;
    while(ptr->next)
        ptr=ptr->next;
    InsertHashNode_Next(ptr,node);
}

void AddHashNode(hashlist list,const unsigned int key,hashlist_node* node){
    if(!IsUnhashed(node))
        return;
    struct hashlist_head* head=&(list->hashtable[key]);
    InsertHashNode_Head(head,node);
    (list->nodenum[key])++; 
}

/*remove hash node*/
static inline void RemoveHashNode(hashlist_node* node){
    assert(!IsUnhashed(node));
    hashlist_node* next=node->next;
    *(node->prev_firstregion)=next;
    if(next)
        next->prev_firstregion=node->prev_firstregion;
    node->next=NULL;
    node->prev_firstregion=NULL;
}

void DeleteHashNode(hashlist list,const unsigned int key,hashlist_node* node){
    if(IsUnhashed(node))
        return;
    RemoveHashNode(node);
    (list->nodenum[key])--;    
}