#ifndef BASE_H
#define BASE_H
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<stdbool.h>

#define ERROR -1

#define bitmap unsigned int
#define flag_bit unsigned char
#define address_bit long long


#define DefaultValue 0

#define SWAP(data1,data2){\
   assert(__builtin_types_compatible_p(typeof(data1),typeof(data2)));\
   const typeof(data1) temp=data1;\
   data1=data2;\
   data2=temp;\
}\

#define ISPOINTER(data)(\
  __builtin_types_compatible_p(typeof(data),typeof(&(*data)))\
)

#define ARRAY_LENGTH(array)(\
{\
    assert(!ISPOINTER(array));\
    sizeof(array)/sizeof(array[0]);\
}\
)

#define OFFSET(host_type,field)(\
{\
    (size_t)&((host_type*)0)->field;\
}\
)

#define HOST_POINTER(host_type,member,addr)\
(\
{\
  typeof(((host_type*)0)->member)* temp = addr;\
  (host_type*)((char*)temp - OFFSET(host_type,member));\
}\
)

#endif
