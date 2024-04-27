#ifndef TIMER_H
#define TIMER_H
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/times.h>
#include<stdbool.h>
#include<stdarg.h>

#define TimeItemSize 8
typedef struct Time{
    struct{
        char t_second[TimeItemSize];
        char t_minute[TimeItemSize];
        char t_hour[TimeItemSize];
        char t_monthday[TimeItemSize];
        char t_month[TimeItemSize];
        char t_year[TimeItemSize];
    } m_calenderdate;
    struct{
        long t_executemsecond;
        long t_msecond;
    } m_timestamp;
}Time_t;
extern void GetFormatDate(char*,const unsigned int,Time_t*);
extern void StartTiming(Time_t*); 
extern long GetTiming(Time_t*);


enum FuncType{
    IA,
    LA,
    FPA,
    DET,
    OPR
};

struct CallFuncContainer;
typedef void (*VariableFuncType)(struct CallFuncContainer*,...);
typedef VariableFuncType FuncWrapper;
typedef void (*IntegerFunc_t)(int*,...);
typedef void (*LongFunc_t)(long*,...);
typedef void (*FloatingFunc_t)(double*,...);
typedef void (*DetectFunc_t)(bool*,...);
typedef void (*OperateFunc_t)(void);
/*实现一个多态回调函数容器*/
typedef union PolyFunction
{
    OperateFunc_t u_operate;
    struct
    {
        IntegerFunc_t func;
        int output;
    }u_intcalc;        
    struct
    {
        LongFunc_t func;
        long output;
    }u_longcalc;        
    struct
    {
        FloatingFunc_t func;
        double output;
    }u_floatcalc;
    struct
    {
        DetectFunc_t func;
        bool result;
    }u_detect;
}PolyFunc_t;

typedef struct CallFuncContainer
{
    enum FuncType m_type;
    union PolyFunction m_func;
    unsigned char m_argc;
}FuncContainer_t;
typedef struct CallFuncContainer* CallFunc;

#define HandleCase(option,operate)\
    case option:\
            operate;\
        break;\

#define HandleFuncInit(Caller,Callee)\
{\
    switch(Caller->m_type)\
    {\
        HandleCase(IA,Caller->m_func.u_intcalc.func = (IntegerFunc_t)(Callee->u_intcalc.func));\
        HandleCase(LA,Caller->m_func.u_longcalc.func = (LongFunc_t)(Callee->u_longcalc.func));\
        HandleCase(FPA,Caller->m_func.u_floatcalc.func = (FloatingFunc_t)(Callee->u_floatcalc.func));\
        HandleCase(DET,Caller->m_func.u_detect.func = (DetectFunc_t)(Callee->u_detect.func));\
        HandleCase(OPR,Caller->m_func.u_operate = (OperateFunc_t)(Callee->u_operate));\
    }\
}

static void HandleFuncExec(CallFunc caller, va_list args) {
    switch (caller->m_type) {
        HandleCase(IA, caller->m_func.u_intcalc.func(&caller->m_func.u_intcalc.output, args))
        HandleCase(LA, caller->m_func.u_longcalc.func(&caller->m_func.u_longcalc.output, args))
        HandleCase(FPA, caller->m_func.u_floatcalc.func(&(caller->m_func.u_floatcalc.output), args))
        HandleCase(DET, caller->m_func.u_detect.func(&(caller->m_func.u_detect.result), args))
        HandleCase(OPR, caller->m_func.u_operate())
        default: break;
    }
}

extern void RegisterFunc(CallFunc,enum FuncType);
extern void RecoverFuncContext(CallFunc);
extern void HookFunc(CallFunc);
extern long GetExecuteMs(CallFunc,PolyFunc_t*,...);


/*采用多级时间轮反馈算法来调度任务*/
struct TimerNode{
    unsigned int index;
    unsigned int rotation;
    struct TimerNode* next;
    CallFunc function;
};

#define TimeWheelSize 128
#define TimeWheelLevelNum 8
#define LowestTimeWheelLevel (TimeWheelLevelNum-1) 
#define HighestTimeWheelLevel 0
#define Priority_t unsigned char
#ifdef _BITS32_
#define INFINITY 0xffffffff
#else
#define INFINITY 0Xffffffffffffffff
#endif

typedef struct TimeWheel{
    unsigned int node_num;
    struct TimerNode* timer_node[TimeWheelSize];
    unsigned char current_index;
    Priority_t current_priority;
}TimeWheel_t;

#define DefaultTimewheel 0
#define MaxUrgentQueueSize 16
typedef struct Timer
{
    unsigned char m_headindex;
    struct TimeWheel m_wheels[TimeWheelLevelNum];
     struct {
        unsigned char s_front;
        unsigned char s_num;
        CallFunc s_queue[MaxUrgentQueueSize];
    } m_urgentqueue;
    unsigned long m_interval;
    Time_t m_time;
}Timer_t;

extern void SetTimer(Timer_t*,const unsigned long);
extern void AddGeneralTask(Timer_t*,CallFunc);
extern void AddUrgentTask(Timer_t*,CallFunc);
extern void AddCycleTask(Timer_t*,CallFunc);
extern void AddConditionalTask(Timer_t*,CallFunc,int);
extern void TickTask(Timer_t*,CallFunc*);
extern void CancelTask(Timer_t*,const unsigned int);
#endif