#include"directionary.h"
#include"bit.h"

#define RadixTreeNodeMask 0x01
#define RadixTreeNodeTag 0x01

#define IsInteralRadixTreeNode(node) (IsBelongtoRoot(RadixTreeNodeTag,RadixTreeNodeMask,node))

#define InitRadixTreeNode(node) {\
    node->slot_shift=0;\
    node->index_shift=0;\
    node->item_counts=0;\
    node->root=NULL;\
    node->parent=NULL;\
    for(int i=0;i<MAX_SLOTSIZE;i++){\
        node->slots[i] = NULL;\
        node->tags[i] = -1;\
    }\
}

#define IsRootNode(node) (node->parent==NULL)

#define MaskNode(node) TagMaskAddr(node,RadixTreeNodeMask)

#define EntryNode(mask_addr) (\
{\
   assert(IsInteralRadixTreeNode(mask_addr));\
   EntryNodeAddr(struct RadixTree_node,mask_addr,RadixTreeNodeMask);\
}\
)

/*判断是否为终端节点（slot中不存在childnode）*/
#define IsTerminalNode(mask_node) (\
{\
   bool establish = true;\
   if(!IsInteralRadixTreeNode(mask_node))\
      establish=false;\
   else{\
      struct RadixTree_node* node = EntryNode(mask_node);\
      for(int index=0;index<MAX_SLOTSIZE;index++)\
         if(IsInteralRadixTreeNode(node->slots[index])){\
            establish = false;\
            break;}\
   }\
   establish==true;\
}\
)

/*判断是否为挂载节点（slot中直接存储item）*/
#define IsMountNode(mask_node) (\
{\
   bool establish = false;\
   if(!IsInteralRadixTreeNode(mask_node))\
      establish=false;\
   else{\
      struct RadixTree_node* node = EntryNode(mask_node);\
      for(int index=0;index<MAX_SLOTSIZE;index++)\
      {\
         void* slot = node->slots[index];\
         if(slot && !IsInteralRadixTreeNode(slot)){\
            establish = true;\
            break;}\
      }\
   }\
   establish==true;\
}\
)

/*判断是否为可压缩的节点（slot中最多只存储单个childnode）*/
#define IsSurplusNode(node) (\
{\
   bool establish = true;\
   flag_bit counts = 0;\
   for(int index=0;index<MAX_SLOTSIZE;index++)\
   {\
      if(counts>1){\
         establish = false;\
         break;\
      }\
      if(IsInteralRadixTreeNode((node)->slots[index]))\
         counts++;\
      else if((node)->slots[index]){\
         establish = false;\
         break;\
      }\
   }\
   establish==true;\
}\
)

#define GenerateCollision(mount_node,shift)(\
{\
   assert(IsMountNode(mount_node));\
   mount_node->slots[shift]!=NULL;\
}\
)

#ifdef _BITS32_
#define TreeNodeShift(BaseIndex,ChildIndex,RemainBits,UnitBits) (\
  UnitShift(unsigned int,BaseIndex,ChildIndex,RemainBits,UnitBits)\
)
#else
#define TreeNodeShift(BaseIndex,ChildIndex,RemainBits,UnitBits) (\
  UnitShift(unsigned long,BaseIndex,ChildIndex,RemainBits,UnitBits)\
)
#endif

#define IsInTreeNodeZone(TargetIndex,BaseIndex,bits) (\
IsBelongtoRange(unsigned long,BaseIndex,TargetIndex,bits)\
)






void BuildRadixTree(RadixTree* tree,const flag_bit unit_num,const flag_bit unit_shift){
   tree->root_slot=NULL;
   tree->unit_num=unit_num;
   tree->unit_shift=unit_shift;
   tree->item_num=0;
}


void MountRadixTreeItem(RadixTree* tree,const unsigned long key,void* item){
   assert(tree);
   const flag_bit total_bits = (tree->unit_shift)*(tree->unit_num);
   const flag_bit unit_bits = tree->unit_shift;
   const flag_bit unit_num = tree->unit_num;
   flag_bit remain_bits = total_bits;
   unsigned long base_index = 0;//用于存储父节点的基序列
   const unsigned long store_index = key;
   flag_bit shift;//用于计算每次向下搜索节点时在slot上的偏移
   struct RadixTree_node* current_node;//存储当前节点地址
   void* ptr;//向下搜索的slot节点   
   //无root节点的情况
   if(tree->root_slot==NULL){
      struct RadixTree_node* new_node=(struct RadixTree_node*)malloc(sizeof(struct RadixTree_node));
      InitRadixTreeNode(new_node);
      new_node->root = tree;
      shift = TreeNodeShift(base_index,store_index,remain_bits,unit_bits);   
      #ifdef DEBUG_MODE
      printf("the slot shift is %d\n",shift);
      #endif
      new_node->slots[shift] = item;
      new_node->tags[shift] = store_index;
      tree->root_slot = MaskNode(new_node);
      return;
   }
   ptr = tree->root_slot;//从根节点开始遍历
   while(IsInteralRadixTreeNode(ptr)){
      current_node = EntryNode(ptr);
      assert(current_node);
      remain_bits = total_bits-unit_bits*(current_node->index_shift);
      base_index |= (current_node->slot_shift)<<(remain_bits-unit_bits);
      if(!IsInTreeNodeZone(store_index,base_index,remain_bits)){
         base_index -= (current_node->slot_shift)<<(remain_bits-unit_bits);
         current_node = current_node->parent;
         if(current_node)
            remain_bits = total_bits-(current_node->index_shift)*unit_bits;
         break;
      }
      (current_node->item_counts)++;
      shift = TreeNodeShift(base_index,store_index,remain_bits,unit_bits);
      ptr = current_node->slots[shift];
      printf("Search...\n"); 
   }
   #ifdef DEBUG_MODE
   printf("the slot shift is %d\n",shift);
   #endif
   assert(current_node);
   assert(shift<MAX_SLOTSIZE);

   //找到符合要求的位置进行挂载
   //直接挂载
   if(!ptr){
      current_node->slots[shift] = item;
      current_node->tags[shift] = store_index;
      return;
   }
   //slot碰撞
   struct RadixTree_node* new_node = (struct RadixTree_node*)malloc(sizeof(struct RadixTree_node));
   InitRadixTreeNode(new_node);
   current_node->slots[shift] = MaskNode(new_node);
   unsigned long mounted_index = current_node->tags[shift];
   new_node->root = tree;
   new_node->parent = current_node;
   new_node->index_shift = MaxCommonValuePrefix(unsigned long,mounted_index,store_index,unit_bits);
   new_node->item_counts = 2;
   remain_bits = total_bits-(new_node->index_shift)*unit_bits;
   base_index |= shift<<(remain_bits-unit_bits);
   current_node->tags[shift] = base_index;
   shift=TreeNodeShift(base_index,store_index,remain_bits,unit_bits);
   new_node->slots[shift] = item;
   new_node->tags[shift] = store_index;
   shift=TreeNodeShift(base_index,mounted_index,remain_bits,unit_bits);
   new_node->slots[shift] = ptr;
   new_node->tags[shift] = mounted_index;
  if(IsInteralRadixTreeNode(ptr)){
      current_node = EntryNode(ptr);
      current_node->slot_shift = shift;
      current_node->parent = new_node;
      new_node->item_counts = current_node->item_counts + 1;
  }
}

void* SearchRadixTreeItem(RadixTree* tree,const unsigned long key_index,struct RadixTree_node** mount_node){
   if(!tree || !tree->root_slot)
      return NULL;
   const unsigned long search_index = key_index; 
   const flag_bit total_bits = (tree->unit_num)*(tree->unit_shift);  
   const flag_bit unit_bits = tree->unit_shift;
   const flag_bit unit_num = tree->unit_num;
   void* ptr = tree->root_slot;
   struct RadixTree_node* current_node;
   flag_bit shift = 0;
   unsigned long base_index = 0;
   unsigned long remain_bits = total_bits;
   while(IsInteralRadixTreeNode(ptr)){
      current_node = EntryNode(ptr);
      base_index |= (current_node->slot_shift)<<(total_bits-(current_node->index_shift+1)*unit_bits);   
      remain_bits = total_bits-unit_bits*(current_node->index_shift);
      shift = TreeNodeShift(base_index,search_index,remain_bits,unit_bits);
      ptr = current_node->slots[shift];
   }
   if(!ptr)
      return NULL;
   if(current_node->tags[shift]!=key_index)
      return NULL;
   (*mount_node) = current_node;
   return ptr;  
}

static inline void CompressRoute(RadixTree* tree,struct RadixTree_node* mount_node){
   if(!IsSurplusNode((mount_node)))
      return;
   flag_bit reserve_index = -1;
   for(flag_bit index=0;index<MAX_SLOTSIZE;index++){
      if(IsInteralRadixTreeNode(mount_node->slots[index])){
         reserve_index = index;
         break; 
      }
   }
   void* reserve_slot = NULL;
   unsigned long reserve_tag = -1;
   if(reserve_index!=-1){
      reserve_slot = mount_node->slots[reserve_index];
      reserve_tag = mount_node->tags[reserve_index];
   }
   struct RadixTree_node* current_node = mount_node;
   struct RadixTree_node* parent_node;
   flag_bit current_shift;
   while(IsSurplusNode(current_node)){
      current_shift = current_node->slot_shift;
      parent_node = current_node->parent;
      free(current_node);
      current_node = parent_node;
   }
   current_node->slots[current_shift] = reserve_slot;
   current_node->tags[current_shift] = reserve_tag;
   struct RadixTree_node* ChildNode = NULL;   
   if(IsInteralRadixTreeNode(reserve_slot))
      ChildNode = EntryNode(reserve_slot);   
   if(ChildNode){
      ChildNode->parent = current_node;
      ChildNode->slot_shift = current_shift;   
   }
}

bool RemoveRadixTreeItem(RadixTree* tree,const unsigned long key_index){
   bool success = false;
   struct RadixTree_node* mount_node;
   flag_bit index;
   if(SearchRadixTreeItem(tree,key_index,&mount_node)){
      success = true;
      for(index=0;index<MAX_SLOTSIZE;index++)
         if(mount_node->tags[index]==key_index)            
            break;
      mount_node->tags[index] = -1;         
   }
   if(!success)
      return false;
   struct RadixTree_node* ptr_node = mount_node;
   while(ptr_node){
      (ptr_node->item_counts)--;
      ptr_node = ptr_node->parent;   
   }
   CompressRoute(tree,mount_node);
   return true;
}


