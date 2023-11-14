#include"timer.h"
#include"base.h"
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/times.h>
#include<stdio.h>
#include<stdarg.h>
#include<assert.h>

static void IntegerToString(int num,char* str,int radix)
{
	char index[]="0123456789ABCDEF";
	unsigned unum;
	int i = 0,j,k;
	if(radix==10&&num<0)
	{
		unum = (unsigned int)-num;
		str[i++] = '-';
	}
	else unum=(unsigned int )num; 
	do
	{
		str[i++] = index[unum%(unsigned)radix];
		unum /= radix; 
	}while(unum);
	str[i] = '\0';
	if(str[0]=='-') k=1;
	else k = 0;
	char temp;
	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];
		str[j]=str[i-1+k-j];
		str[i-1+k-j]=temp;
	}
}

void GetFormatDate(char* time_item,const unsigned int length,Time* time_object){
    time_t current_date;
    time(&current_date);
    struct tm* local_date = localtime(&current_date);
    unsigned int index = 0;
    index = sprintf(time_item,"%d-",local_date->tm_year+1900);
	index +=  sprintf(time_item+index,"%d-",local_date->tm_mon+1);
	index +=  sprintf(time_item+index,"%d-",local_date->tm_mday);
    index +=  sprintf(time_item+index,"%d:",(local_date->tm_hour+16)%24);
    index +=  sprintf(time_item+index,"%d:",local_date->tm_min);
	index +=  sprintf(time_item+index,"%d",local_date->tm_sec);     
	assert(index<length);
	if(time_object)
	{
		IntegerToString(local_date->tm_year+1900,time_object->m_calenderdate.t_year,10);
 		IntegerToString(local_date->tm_mon+1,time_object->m_calenderdate.t_month,10);
		IntegerToString(local_date->tm_mday,time_object->m_calenderdate.t_monthday,10);
		IntegerToString((local_date->tm_hour+16)%24,time_object->m_calenderdate.t_hour,10);
		IntegerToString(local_date->tm_min,time_object->m_calenderdate.t_minute,10);
		IntegerToString(local_date->tm_sec,time_object->m_calenderdate.t_second,10);
	}     
}

void StartTiming(Time* time_object){
	struct timeval time_value;
	gettimeofday(&time_value,NULL);
	time_object->m_timestamp.t_msecond = time_value.tv_sec*1000 + time_value.tv_usec/1000;
	time_object->m_timestamp.t_executemsecond = 0;
}

void ExecuteFunc(CallFunc p_caller){

}

long GetExecuteMs(CallFunc p_caller,...){
	va_list arg_list;
	va_start(arg_list,p_caller);
	struct timeval time_value;
	gettimeofday(&time_value,NULL);
	long start_ms = time_value.tv_sec*1000 + time_value.tv_usec/1000;
	p_caller->m_func(p_caller->m_ArgsNum,arg_list);
	gettimeofday(&time_value,NULL);
	long end_ms = time_value.tv_sec*1000 + time_value.tv_usec/1000;
	long lag_ms = end_ms-start_ms;
	va_end(arg_list);
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
	(newnode->function)->m_ArgsNum = func->m_ArgsNum;
	(newnode->function)->m_func = func->m_func;
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
		timer->m_wheels[level] = NULL;\
	timer->m_interval = INFINITY;\
	StartTiming(&(timer->m_time));\
}

void SetTimer(Timer* p_timer,const unsigned long p_interval){
	assert(p_timer);
	InitTimer(p_timer);
	p_timer->m_interval = p_interval;
}

void AddGeneralTask(Timer* p_timer,CallFunc p_func){
	struct TimeWheel* current_wheel = p_timer->m_wheels[p_timer->m_headindex];
	if(!current_wheel){
		current_wheel = (struct TimeWheel*)malloc(sizeof(struct TimeWheel));
		current_wheel->node_num = 0;
		current_wheel->current_index = 0;
	}
	InsertTimerNode(current_wheel,p_func,current_wheel->node_num);
	current_wheel->node_num++;
}

void AddUrgentTask(Timer* p_timer,CallFunc p_func){

}

void AddConditionalTask(Timer* p_timer,CallFunc p_func,int p_level){

}

#define DownGrading(timewheel)\
{\
	if(timewheel->current_priority<LowestTimeWheelLevel)\
		timewheel->current_priority++;\
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

void TickTask(Timer* p_timer,CallFunc* p_funcvector){
	struct TimeWheel* wheel = p_timer->m_wheels[p_timer->m_headindex];
	if(p_timer->m_time.m_timestamp.t_executemsecond>=p_timer->m_interval)
	{
		DownGrading(wheel);
		unsigned int index = 0;
		while(index<TimeWheelLevelNum && index!=p_timer->m_headindex)
		{
			
		}
	}
	
}

void CancelTask(Timer* p_timer,const unsigned int p_index){
	struct TimeWheel* wheel = p_timer->m_wheels[p_timer->m_headindex];
	DeleteTimerNode(wheel,p_index);
}