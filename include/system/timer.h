#ifndef TIMER_H
#define TIMER_H
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/times.h>
#include<stdbool.h>
#include<stdarg.h>

#define TimeItemSize 8
struct TimeType{
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
};
typedef struct TimeType Time;
extern void GetFormatDate(char*,const unsigned int,Time*);
extern void StartTiming(Time*); 
extern long GetTiming(Time*);


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
typedef void (*IntegerCaculateFuncType)(int*,...);
typedef void (*LongCaculateFuncType)(long*,...);
typedef void (*FloatingCaculateFuncType)(double*,...);
typedef void (*DetectFuncType)(bool*,...);
typedef void (*OperateFuncType)(void*);

struct CallFuncContainer{
    enum FuncType m_type;
    union
    {
        OperateFuncType u_operate;
        struct
        {
            IntegerCaculateFuncType int_func;
            int rv;
            int last;
        }   u_intcalc;        
        struct
        {
             LongCaculateFuncType long_func;
             long rv;
             long last;
        }   u_longcalc;        
        struct
        {
            FloatingCaculateFuncType floating_func;
            double rv;
            double last;
        }   u_floatcalc;
        struct
        {
              DetectFuncType detect_func;
              bool rv;
              long last;
        }   u_detect;
    }   m_func;
    unsigned char m_argc;
};
typedef struct CallFuncContainer* CallFunc;

#define HandleCase(option,operate)\
{\
    case option:\
        {operate;}\
        break;\
}

#define HandleFuncType(Caller,Args...)\
{\
    switch(Caller->m_type)\
    {\
        HandleCase(IA,Caller->m_func.u_intcalc.int_func(&(Caller->m_func.u_intcalc.rv),Args));\
        HandleCase(LA,Caller->m_func.u_longcalc.long_func(&(Caller->m_func.u_longcalc.rv),Args));\
        HandleCase(FPA,Caller->m_func.u_floatcalc.floating_func(&(Caller->m_func.u_floatcalc.rv),Args));\
        HandleCase(DET,Caller->m_func.u_detect.detect_func(&(Caller->m_func.u_detect.rv),Args));\
        HandleCase(OPR,Caller->m_func.u_operate(Args));\
    }\
}

extern void RegisterFunc(CallFunc,enum FuncType);
extern void RecoverFuncContext(CallFunc);
extern void HookFunc(CallFunc);

#define GetExecuteMs(Caller,Object,Args...)\
(\
{\
	struct timeval time_value;\
	gettimeofday(&time_value,NULL);\
	long start_ms = time_value.tv_sec*1000 + time_value.tv_usec/1000;\
	typeof(Object)* obj_addr = &Object;\
    HandleFuncType(Caller,obj_addr);\
    gettimeofday(&time_value,NULL);\
	long end_ms = time_value.tv_sec*1000 + time_value.tv_usec/1000;\
	long lag_ms = end_ms-start_ms;\
	lag_ms;\
}\
)


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
#define priority unsigned char
#ifdef _BITS32_
#define INFINITY 0xffffffff
#else
#define INFINITY 0Xffffffffffffffff
#endif

struct TimeWheel{
    unsigned int node_num;
    struct TimerNode* timer_node[TimeWheelSize];
    unsigned char current_index;
    priority current_priority;
};

#define DefaultTimewheel 0
#define MaxUrgentQueueSize 16
struct TimerType{
    unsigned char m_headindex;
    struct TimeWheel m_wheels[TimeWheelLevelNum];
     struct {
        unsigned char s_front;
        unsigned char s_num;
        CallFunc s_queue[MaxUrgentQueueSize];
    } m_urgentqueue;
    unsigned long m_interval;
    Time m_time;
};
typedef struct TimerType Timer;

extern void SetTimer(Timer*,const unsigned long);
extern void AddGeneralTask(Timer*,CallFunc);
extern void AddUrgentTask(Timer*,CallFunc);
extern void AddCycleTask(Timer*,CallFunc);
extern void AddConditionalTask(Timer*,CallFunc,int);
extern void TickTask(Timer*,CallFunc*);
extern void CancelTask(Timer*,const unsigned int);
#endif