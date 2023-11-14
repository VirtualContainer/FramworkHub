#ifndef TIMER_H
#define TIMER_H

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

typedef void (*VariableFuncType)(unsigned char,...);
struct CallFuncContainer{
    VariableFuncType m_func;
    unsigned char m_ArgsNum;
};
typedef struct CallFuncContainer* CallFunc;
extern void ExecuteFunc(CallFunc);
extern long GetExecuteMs(CallFunc,...);

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

struct TimerType{
    unsigned int m_headindex;
    struct TimeWheel* m_wheels[TimeWheelLevelNum];
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