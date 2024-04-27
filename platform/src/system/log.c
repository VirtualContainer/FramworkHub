#include"log.h"
#include<unistd.h>
#include<assert.h>
#include<stdlib.h>


void InitLogger(struct Logger* p_logger,enum LogLevel p_level,enum LogOutputFlag p_output)
{      
    p_logger->m_eventnum = 0;
    p_logger->m_level = p_level;
    p_logger->m_outputflag = p_output;
    int index;
    for(index=0;index<MaxLogEventNum;index++)
        p_logger->m_events[index] = NULL;
    pthread_rwlock_init(&p_logger->m_filesync,NULL);
    pthread_mutex_init(&p_logger->m_terminalsync,NULL);      
}

typedef void (*LogAppenderWay)(LogEvent_t*,Logger_t*);
typedef void (*LogEnlighten)(FILE*);

void DebugLighten(FILE* file)
{
    if(file)
    {
        fprintf(file,"\033[32m%s\t\033[0m","[ Debug ]");        
        return;
    }
    printf("\033[32m%s\t\033[0m","[ Debug ]");
}
void InformLighten(FILE* file)
{
    if(file)
    {
        fprintf(file,"\033[32m%s\t\033[0m","[ Inform ]");        
        return;
    }
    printf("\033[34m%s\t\033[0m","[ Inform ]");
}
void WarnLighten(FILE* file)
{
    if(file)
    {
        fprintf(file,"\033[32m%s\t\033[0m","[ Warn ]");        
        return;
    }
    printf("\033[33m%s\t\033[0m","[ Warn ]");
}
void ErrorLighten(FILE* file)
{
    if(file)
    {
        fprintf(file,"\033[32m%s\t\033[0m","[ Error ]");        
        return;
    }
    printf("\033[35m%s\t\033[0m","[ Error ]");
}
void CrashLighten(FILE* file)
{
    if(file)
    {
        fprintf(file,"\033[32m%s\t\033[0m","[ Crash ]");        
        return;
    }
    printf("\033[31m%s\t\033[0m","[ Crash ]");
}

static void TerminalLog(LogEvent_t* p_event,Logger_t* p_logger){
    LogEnlighten level_enlightens[5];
    level_enlightens[0] = &DebugLighten;
    level_enlightens[1] = &InformLighten;
    level_enlightens[2] = &WarnLighten;
    level_enlightens[3] = &ErrorLighten;
    level_enlightens[4] = &ErrorLighten;
    assert(p_event->Level!=Log_Unknow);       
    pthread_mutex_lock(&p_logger->m_terminalsync);
    int index = p_event->Level;
    if(level_enlightens[index])
        level_enlightens[index](NULL);
    #ifdef DEBUG_MODE
    printf(" %-7s\t %-7s\t %-7d\t",p_event->FileName,p_event->FuncName,p_event->LineNumber);
    #endif
    printf(" %-7s\t %-7s\t %-7s\t %-7s\t %-15s\t",p_event->SpaceName,p_event->ProcessName,p_event->ThreadName,p_event->TimeItem,p_event->EventContent);
    printf("\n");
    pthread_mutex_unlock(&p_logger->m_terminalsync);
}

#define PathLength 64
#define FileNameLength 16
static void SystemFileLog(LogEvent_t* p_event,Logger_t* p_logger){    
    char DirectoryPath[PathLength];
    getcwd(DirectoryPath,PathLength);
    char FileName[FileNameLength] = "log.txt";    
    char LogFilePath[PathLength];
    sprintf(LogFilePath,"%s%s%s",DirectoryPath,"/",FileName);
    FILE* file = fopen(LogFilePath,"a+");
    assert(file);
    pthread_rwlock_wrlock(&p_logger->m_filesync);
    #ifdef DEBUG_MODE
    fprintf(file,"%-7s\t%-7s\t%-7d\t",p_event->FileName,p_event->FuncName,p_event->LineNumber);
    #endif
    fprintf(file,"%-7s\t%-7s\t%-7s\t%-7s\t%-15s\t",p_event->SpaceName,p_event->ProcessName,p_event->ThreadName,p_event->TimeItem,p_event->EventContent);
    fprintf(file,"\033[0m\n");
    //fclose(file);
    pthread_rwlock_unlock(&p_logger->m_filesync);    
}

void LogLogger(struct Logger* p_logger,struct LogEvent* p_event,bool p_backup){
    assert(p_logger);
    assert(p_event);
    if(p_event->Level<p_logger->m_level)
        return;
    LogAppenderWay appender_ways[2];
    appender_ways[0]=&TerminalLog;
    appender_ways[1]=&SystemFileLog;
    const int index = p_logger->m_outputflag;
    (appender_ways[index])(p_event,p_logger);
    if(p_backup)
    {
        p_logger->m_events[p_logger->m_eventnum] = (struct LogEvent*)malloc(sizeof(struct LogEvent));
        assert(p_logger->m_events[p_logger->m_eventnum]);
        struct LogEvent* current_event = p_logger->m_events[p_logger->m_eventnum];
        current_event->Level = p_event->Level;
        #ifdef DEBUG_MODE
        strcpy(current_event->FileName,p_event->FileName);
        strcpy(current_event->FuncName,p_event->FuncName);
        current_event->LineNumber = p_event->LineNumber;
        #endif
        p_logger->m_eventnum++;
        strcpy(current_event->SpaceName,p_event->SpaceName);
        strcpy(current_event->ProcessName,p_event->ProcessName);
        strcpy(current_event->ThreadName,p_event->ThreadName);
        strcpy(current_event->TimeItem,p_event->TimeItem);
        strcpy(current_event->EventContent,p_event->EventContent); 
    }
}

void RuinLogger(struct Logger* p_logger){
    assert(p_logger);
    if(p_logger->m_eventnum)
    {
        unsigned int index;
        for(index = 0;index < p_logger->m_eventnum;index++){
            free(p_logger->m_events[index]);
            p_logger->m_events[index] = NULL;
        }            
    }
    p_logger->m_eventnum = 0;
    p_logger->m_level = Log_Unknow;
    p_logger->m_outputflag = Unspecified;
    pthread_mutex_destroy(&p_logger->m_terminalsync);
    pthread_rwlock_destroy(&p_logger->m_filesync);    
}