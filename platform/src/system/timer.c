#include"timer.h"
#include"base.h"
#include<stdio.h>
#include<assert.h>
#include"types.h"

void GetFormatDate(char* time_item,const unsigned int length,Time_t* time_object){
    time_t current_date;
    time(&current_date);
    struct tm* local_date = localtime(&current_date);
    unsigned int index = 0;
    index = sprintf(time_item,"%d:",local_date->tm_year+1900);
	index +=  sprintf(time_item+index,"%d:",local_date->tm_mon+1);
	index +=  sprintf(time_item+index,"%d:",local_date->tm_mday);
    index +=  sprintf(time_item+index,"%d:",(local_date->tm_hour+15)%24);
    index +=  sprintf(time_item+index,"%d:",local_date->tm_min);
	index +=  sprintf(time_item+index,"%d",local_date->tm_sec);     
	assert(index<length);
	if(time_object)
	{
		IntegerToString(local_date->tm_year+1900,time_object->m_calenderdate.t_year,10);
 		IntegerToString(local_date->tm_mon+1,time_object->m_calenderdate.t_month,10);
		IntegerToString(local_date->tm_mday,time_object->m_calenderdate.t_monthday,10);
		IntegerToString((local_date->tm_hour+15)%24,time_object->m_calenderdate.t_hour,10);
		IntegerToString(local_date->tm_min,time_object->m_calenderdate.t_minute,10);
		IntegerToString(local_date->tm_sec,time_object->m_calenderdate.t_second,10);
	}     
}

void StartTiming(Time_t* time_object)
{
	struct timeval time_value;
	gettimeofday(&time_value,NULL);
	time_object->m_timestamp.t_msecond = time_value.tv_sec*1000 + time_value.tv_usec/1000;
	time_object->m_timestamp.t_executemsecond = 0;
}

long GetTiming(Time_t* time_object)
{
	struct timeval time_value;
	gettimeofday(&time_value,NULL);
	long elapse_ms;
	elapse_ms = time_value.tv_sec*1000+time_value.tv_usec/1000-time_object->m_timestamp.t_msecond;
	time_object->m_timestamp.t_executemsecond = elapse_ms;
	return elapse_ms;
}

void RegisterFunc(CallFunc p_caller,enum FuncType p_type)
{

}

void RecoverFuncContext(CallFunc p_caller)
{

}

void HookFunc(CallFunc p_caller)
{

}

/*计算该函数执行所需要的事件,Caller为多态回调函数容器指针*/
long GetExecuteMs(CallFunc caller, PolyFunc_t* callee, ...) {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    long start_ms = time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    HandleFuncInit(caller,callee);
    va_list args;
    va_start(args, callee);
    HandleFuncExec(caller, args);
    va_end(args);
    gettimeofday(&time_value, NULL);
    long end_ms = time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    long lag_ms = end_ms - start_ms;
    return lag_ms;
}

#define InitTimerNode(node)\
{\
	node->index = 0;\
	node->rotation = 0;\
	node->next = NULL;\
}

static void InsertTimerNode(struct TimeWheel* wheel,CallFunc func,const unsigned int index){
	struct TimerNode* newnode = (struct TimerNode*)malloc(sizeof(struct TimerNode));
	InitTimerNode(newnode);
	newnode->index = index;
	const unsigned int cycle = TimeWheelSize;
	newnode->rotation = index/cycle;
	newnode->function = func;
	const unsigned int location = index%cycle;
	struct TimerNode** ptr_node = &(wheel->timer_node[location]);
	while(*ptr_node)
		ptr_node = &((*ptr_node)->next);
	*ptr_node = newnode; 	
}

static unsigned int TickTimerNode(struct TimeWheel* wheel,CallFunc* func_vector){
	struct TimerNode** ptrnode = &(wheel->timer_node[wheel->current_index]);
	unsigned int index = 0;
	while (*ptrnode)
	{
		struct TimerNode* currentnode = *ptrnode;
		if(currentnode->rotation==0)
		{
			func_vector[index++] = currentnode->function;
			*ptrnode = (currentnode->next);
		}
		else
		{
			(currentnode->rotation)--;
			ptrnode = &(currentnode->next);
		}		
	}
	wheel->current_index = (wheel->current_index+1)%TimeWheelSize;
	return index;	
}

static void DeleteTimerNode(struct TimeWheel* wheel,const unsigned int index){
	const unsigned int location = index%TimeWheelSize;
	struct TimerNode** ptrnode = &(wheel->timer_node[location]);
	while(*ptrnode)
	{
		struct TimerNode* currentnode = *ptrnode;
		if(currentnode->index==index)
		{
			*ptrnode = (currentnode->next);
			free(currentnode);
			currentnode = NULL;
			return;
		}
		ptrnode = &(currentnode->next);
	}
}

#define InitTimer(timer)\
{\
	unsigned int level;\
	timer->m_headindex = 0;\
	for(level=0;level<TimeWheelLevelNum;level++)\
		(timer->m_wheels[level]).current_priority = HighestTimeWheelLevel;\
	timer->m_interval = INFINITY;\
	timer->m_urgentqueue.s_front = 0;\
	timer->m_urgentqueue.s_num = 0;\
	StartTiming(&(timer->m_time));\
}

void SetTimer(Timer_t* p_timer,const unsigned long p_interval){
	assert(p_timer);
	InitTimer(p_timer);
	p_timer->m_interval = p_interval;
}

void AddGeneralTask(Timer_t* p_timer,CallFunc p_func){
	struct TimeWheel* current_wheel = &(p_timer->m_wheels[p_timer->m_headindex]);
	InsertTimerNode(current_wheel,p_func,current_wheel->node_num);
	current_wheel->node_num++;
}

void AddUrgentTask(Timer_t* p_timer,CallFunc p_func){
	if(p_timer->m_urgentqueue.s_num==MaxUrgentQueueSize)
		return;
	unsigned char location = (p_timer->m_urgentqueue.s_front+p_timer->m_urgentqueue.s_num)%MaxUrgentQueueSize;
	p_timer->m_urgentqueue.s_queue[location] = p_func;
	p_timer->m_urgentqueue.s_num++;
}

void AddConditionalTask(Timer_t* p_timer,CallFunc p_func,int p_level){

}

 void AddCycleTask(Timer_t* p_timer,CallFunc p_func){

 }

#define DownGrading(timewheel)\
{\
	if(timewheel->current_priority<LowestTimeWheelLevel)\
		timewheel->current_priority++;\
}

static void HandleUrgentTask(Timer_t* p_timer){
	if(p_timer->m_urgentqueue.s_num==0)
		return;
	unsigned char index;

}

#define IsLdling(timewheel)(\
{\
	bool result = true;\
	unsigned int index;\
	for(index=0;index<TimeWheelSize;index)\
		if(timewheel->timer_node[index])\
		{\
			result = false;\
			break;\
		}\
	(result==true)\
}\
)

#define IsHigherPriority(object,target_priority)(\
	object.current_priority<target_priority\
)

void TickTask(Timer_t* p_timer,CallFunc* p_FuncVector){	
	
	struct TimeWheel* current_wheel = &(p_timer->m_wheels[p_timer->m_headindex]);
	GetTiming(&p_timer->m_time);	
	if(p_timer->m_time.m_timestamp.t_executemsecond>=p_timer->m_interval)
	{
		DownGrading(current_wheel);
		unsigned char index = 0,result = -1;
		Priority_t higher_priority = LowestTimeWheelLevel;
		while(index<TimeWheelLevelNum && index!=p_timer->m_headindex)
		{
			if(IsHigherPriority((p_timer->m_wheels[index]),higher_priority))
			{
				result = index;
				higher_priority = p_timer->m_wheels[index].current_priority;
			}
			index++;
		}
		if(result==-1)
		{
			for(index=0;index<TimeWheelLevelNum;index++)
				p_timer->m_wheels[index].current_priority = HighestTimeWheelLevel;
			p_timer->m_headindex = DefaultTimewheel; 
		}
		p_timer->m_headindex = result;
		StartTiming(&p_timer->m_time);
	}
	current_wheel = &(p_timer->m_wheels[p_timer->m_headindex]);
	TickTimerNode(current_wheel,p_FuncVector);
}


void CancelTask(Timer_t* p_timer,const unsigned int p_index){
	struct TimeWheel* wheel = &(p_timer->m_wheels[p_timer->m_headindex]);
	DeleteTimerNode(wheel,p_index);
}