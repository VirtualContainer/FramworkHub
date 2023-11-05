#ifndef BIT_H
#define BIT_H
#include"base.h"


#define PERWORDLENGTH 32

#define BIT_MASK(index) ((1<<(index%PERWORDLENGTH)))
#define WORD_INDEX(index) (index%PERWORDLENGTH)

static inline void SetBitMask(void* start_addr,unsigned int index){
    unsigned long* target_addr = (unsigned long*)start_addr+WORD_INDEX(index);
    unsigned int mask = BIT_MASK(index);
    (*target_addr) |= mask;
}

static inline void ClearBitMask(void* start_addr,unsigned int index){
    unsigned long* target_addr = (unsigned long*)start_addr+WORD_INDEX(index);
    unsigned int mask = BIT_MASK(index);
    (*target_addr) &= ~mask;
}

#define TagMaskAddr(obj_pointer,mask)(\
{\
    uintptr_t obj_addr = (uintptr_t)obj_pointer;\
    uintptr_t tag_addr = obj_addr | mask;\
    void* tag_pointer = (void*)tag_addr;\
    tag_pointer;\
}\
)

#define EntryNodeAddr(type,mask_addr,mask)(\
{\
    uintptr_t entry_addr = (uintptr_t)mask_addr & ~mask;\
    type* entry_pointer = (type*)entry_addr;\
    entry_pointer;\
}\
)

#define IsBelongtoRoot(tag,mask,pointer)(\
{\
    bool establish = true;\
    if(pointer==NULL)\
        establish = false;\
    else{\
        uintptr_t addr = (uintptr_t)pointer;\
        if((addr&mask)!=tag)\
            establish = false;\
    }\
    establish==true;\
}\
)

#define MaxCommonValuePrefix(type,value1,value2,unit_size)(\
{\
    unsigned char counts = 0;\
    type temp_value = ~(value1^value2);\
    while(temp_value){\
        temp_value >>= 1;\
        counts++;\
    }\
    counts = counts/unit_size;\
    counts;\
}\
)

#define MaxCommonAddressPrefix(type,addr1,addr2,unit_size)(\
{\
    bool ispointer_1 = _Generic((addr1),type*:true,default:false);\
    bool ispointer_2 = _Generic((addr2),type*:true,default:false);\
    if(!ispointer_1 || !ispointer_2)\
        exit(-1);\
    unsigned char counts = 0;\
    uintptr_t data1 = (uintptr_t) addr1;\
    uintptr_t data2 = (uintptr_t) addr2;\
    uintptr_t temp_data = ~(data1^data2);\
    while(temp_data){\
        temp_data >>= 1;\
        counts++;\
    }\
    counts = counts/unit_size;\
    counts;\
}\
)

#define IsBelongtoRange(type,RootIndex,ChildIndex,bits)(\
{\
    type CeilingIndex = RootIndex|((1<<bits)-1);\
    bool belong=true;\
    if(ChildIndex < RootIndex || ChildIndex >CeilingIndex)\
        belong=false;\
    belong==true;\
}\
)

#define UnitShift(type,RootIndex,ChildIndex,RemainBits,UnitBits)(\
{\
    type unit_shift = (ChildIndex-RootIndex)>>(RemainBits-UnitBits);\
    unit_shift;\
}\
)

#endif