#include"init.h"



int main(int argc,char* argv[])
{
    InitKernel();
    sleep(1);
    TestKernel();
    Thread_t thread;
    InitThread(&thread,1,"test",NULL);
    CreateThread(&thread);
    HandleThread(&thread,NULL);
    
    return 0;
}