#ifndef NUM_H
#define NUM_H

#define MAX(value1,value2)\
(\
{\
    typeof(value1) _value1=value1;\
    typeof(value2) _value2=value2;\
    _value1>_value2?_value1:_value2;\
}\
)
#define MIN(value1,value2)\
(\
{\
    typeof(value1) _value1=value1;\
    typeof(value2) _value2=value2;\
    _value1<_value2?_value1:_value2;\
}\
)

/*求解最大公约数*/
extern int GCD(const int a,const int b);

extern void CongruenceEquation(const int a,const int b,const int n,int* solution,int* size);

static int LMU(const int value1,const int value2)
{

}


#endif