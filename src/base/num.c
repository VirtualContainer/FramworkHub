#include"num.h"

#include"num.h"

int GCD(const int value1,const int value2)
{
    int result,temp;
    int _value1 = value1;
    int _value2 = value2;
    while(_value2)
    {
        temp = _value2;
        _value2 = _value1%_value2;
        _value1 = temp;
    }
    result = _value1;
    return result;
}

static int ExtendGCD(int value1,int value2,int* x,int* y)
{
    if(value2==0)
    {
        *x = 1;
        *y = 0;
        return value1;
    }
    int x1,y1,gcd;
    gcd = ExtendGCD(value2,value1%value2,&x1,&y1);
    *x = y1;
    *y = x1-value1/value2*y1;
    return gcd;
}

void CongruenceEquation(const int a,const int b,const int n,int* solution,int* size)
{
    int x,y,x0;
    int gcd = ExtendGCD(a,n,&x,&y);
    if(b%gcd)
    {
        *size = 0;
        return;
    }
    *size = gcd;
    x0 = b/gcd*x;
    solution[0] = x0;
    int i;
    for(i=1;i<gcd;i++)
        solution[i] = x0+i*n/gcd;
}