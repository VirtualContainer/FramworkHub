#ifndef LOG_H
#define LOG_H


#define NameLen 16
#define StringLen 32
#define ItemLen 32

enum LogLevel{
    Log_Debug = 0,
    Log_Inform,
    Log_Warn,
    Log_Error,
    Log_Crash,
    Log_Unknow
};

typedef struct LogEvent{
    enum LogLevel Level;
    char SpaceName[NameLen];
    char ProcessName[NameLen];
    char ThreadName[NameLen];
    char TimeItem[ItemLen];
    char EventContent[StringLen]; 
    #ifdef DEBUG_MODE
    char FileName[NameLen];
    char FuncName[NameLen];
    unsigned int LineNumber;
    #endif   
}LogEvent_t;



enum LogOutputFlag{
    Terminal = 0,
    File,
    Unspecified
};

#include<pthread.h>
#include<stdatomic.h>
#include<stdbool.h>

/*日志本*/
#define MaxLogEventNum 1024
typedef struct Logger{
    struct LogEvent* m_events[MaxLogEventNum];
    atomic_int     m_eventnum;
    enum LogLevel    m_level;
    enum LogOutputFlag m_outputflag;
    pthread_mutex_t m_terminalsync;
    pthread_rwlock_t m_filesync;
}Logger_t;


extern void InitLogger(struct Logger*,enum LogLevel,enum LogOutputFlag);
extern void LogLogger(struct Logger*,struct LogEvent*,bool);
extern void RuinLogger(struct Logger*);

#include <string.h>
#include <stdio.h>
#ifdef DEBUG_MODE
#define Log(logger,level,space,process,thread,time,content,backup)\
{\
    struct LogEvent event;\
    memset(event.FileName,0,NameLen);\
    memset(event.FuncName,0,NameLen);\
    memset(event.SpaceName,0,NameLen);\
    memset(event.ProcessName,0,NameLen);\
    memset(event.ThreadName,0,NameLen);\
    memset(event.TimeItem,0,ItemLen);\
    memset(event.EventContent,0,StringLen);\
    event.Level = level;\
    char path[64] = {'\0'};\
    strcpy(path,__FILE__);\
    char* token;\
    char* file;\
    const char delim[] = "/";\
    token = strtok(path,delim);\
    while(token)\
    {\
        if(*token!='\0')\
            file = token;\
        token = strtok(NULL,delim);\
    }\
    strcpy(event.FileName,file);\
    strcpy(event.FuncName,__func__);\
    event.LineNumber = (unsigned int)__LINE__;\
    strcpy(event.SpaceName,space);\
    strcpy(event.ProcessName,process);\
    strcpy(event.ThreadName,thread);\
    strcpy(event.TimeItem,time);\
    strcpy(event.EventContent,content);\
    LogLogger(logger,&event,backup);\
}
#else
#define Log(logger,level,space,process,thread,time,content,backup)\
{\
    struct LogEvent event;\
    event.Level = level;\
    memset(event.SpaceName,0,NameLen);\
    memset(event.ProcessName,0,NameLen);\
    memset(event.ThreadName,0,NameLen);\
    memset(event.TimeItem,0,ItemLen);\
    memset(event.EventContent,0,StringLen);\
    strcpy(event.SpaceName,space);\
    strcpy(event.ProcessName,process);\
    strcpy(event.ThreadName,thread);\
    strcpy(event.TimeItem,time);\
    strcpy(event.EventContent,content);\
    LogLogger(logger,&event,backup);\
}
#endif

#endif