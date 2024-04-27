#include "init.h"

 Logger_t g_logger;
 Time_t g_time;

#ifdef DEBUG_MODE
static void idle()
{
    sleep(1);
}

typedef struct TestModel
{
  unsigned index;
}TestModel_t;
#endif

 void InitKernel()
 {   
   char time_item[ItemLen];
   memset(time_item,0,ItemLen);
   GetFormatDate(time_item,ItemLen,&g_time);
   printf("[ Level ]\t"); 
   #ifdef DEBUG_MODE
   printf("[ File ]\t[ Function ]\t[ Line ]\t");
   #endif
   printf("[ Space ]\t[ Process ]\t[ Thread ]\t  [ Data Time ]\t\t[ Event Message ]\t\n");
   InitLogger(&g_logger,Log_Debug,Terminal);
   Logger_t* logger_addr = &g_logger;
   Log(logger_addr,Log_Debug,"kernel","root","init",time_item,"start kernel",false);   
   
   CreatePG("init",NULL);
 }

#ifdef DEBUG_MODE
 void TestKernel()
 {
    
    FuncContainer_t idle_callers = {.m_type=OPR,.m_argc=0};
    CallFunc caller_addr = &idle_callers;
    PolyFunc_t callee = {.u_operate = &idle};
    GetExecuteMs(caller_addr,&callee);

    Allocater_t allocater;
    InitAllocater(&allocater);
    TestModel_t* model;
    Allocate(&allocater,(void**)&model,sizeof(TestModel_t));
    CloseAllocater(&allocater);
 }
#endif
