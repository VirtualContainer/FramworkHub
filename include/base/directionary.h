#ifndef DIRECTIONARY_H
#define DIRECTIONARY_H
#include"base.h"

#define MAX_SLOTSIZE 256


struct Item;
struct RadixTree_root;

struct RadixTree_node{
    flag_bit slot_shift;//当前节点在父节点slots上的偏移量
    flag_bit index_shift;//当前节点index的偏移量
    unsigned int item_counts;//当前节点挂载对象数量
    struct RadixTree_root* root;//root指针
    struct RadixTree_node* parent;//父节点
    void* slots[MAX_SLOTSIZE];//存储挂载item的地址
    unsigned long tags[MAX_SLOTSIZE];//记录挂载item和node的序列号
};

struct RadixTree_root{
    void* root_slot;
    flag_bit unit_num;
    flag_bit unit_shift;
    unsigned int item_num;
};

typedef struct RadixTree_root RadixTree;

extern void BuildRadixTree(RadixTree*,const flag_bit,const flag_bit);
extern void MountRadixTreeItem(RadixTree*,const unsigned long,void*);
extern void* SearchRadixTreeItem(RadixTree*,const unsigned long,struct RadixTree_node**);
extern bool RemoveRadixTreeItem(RadixTree*,const unsigned long);

struct Item{
   struct RadixTree_root* m_Root; 
   void* m_MountPoint;
   unsigned long m_Index;
   void* m_Obj;
};

struct directionary;
typedef address_bit (*LookupFunction)(struct directionary*,long long index); 

struct directionary
{
    unsigned int index;
    RadixTree tree;
    
};






#endif