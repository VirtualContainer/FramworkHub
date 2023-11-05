#ifndef BASE_H
#define BASE_H
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<assert.h>

#define bool unsigned char
#define true 1
#define false 0
#define error -1

#define bitmap unsigned int
#define flag_bit unsigned char
#define address_bit long long


#define DefaultValue 0
#define MAX(value1,value2)(\
{\
    typeof(value1) _value1=value1;\
    typeof(value2) _value2=value2;\
    _value1>_value2?_value1:_value2;\
}\
)
#define MIN(value1,value2)(\
{\
    typeof(value1) _value1=value1;\
    typeof(value2) _value2=value2;\
    _value1<_value2?_value1:_value2;\
}\
)
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

#define OFFSET(container_type,field)(\
{\
    (size_t)&((container_type*)0)->field;\
}\
)
#define CONTAINER_POINTER(container_type,member,pointer)(\
{\
   typeof(((container_type*)0)->member)* temp = pointer;\
  (container_type*)((char*)temp - OFFSET(container_type,member));\
}\
)


#endif
