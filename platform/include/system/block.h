#ifndef BLOCK_H
#define BLOCK_H

struct Block;

extern int BlockInit(struct Block* block,void* memory,const int size,const int num);
extern void* BlockAlloc(struct Block* block,const unsigned ms);
extern void BlockFree(struct Block* block,void* node);

#endif