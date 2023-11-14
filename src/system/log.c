#include"log.h"
#include<unistd.h>
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void InitLogger(struct Logger* p_logger,enum LogLevel p_level,enum LogOutputFlag p_output){
    p_logger->m_eventnum = 0;
    p_logger->m_level = p_level;
    p_logger->m_outputflag = p_output;
    int index;
    for(index=0;index<LogEventNum;index++)
        p_logger->m_events[index] = NULL;
}

typedef void (*LogAppenderWay)(struct LogEvent*);
typedef void (*LogEnlighten)(void);

void DebugLighten(void){
    printf("\033[32m%-31s\t","Debug");
}
void InformLighten(void){
    printf("\033[34m%-31s\t","Inform");
}
void WarnLighten(void){
    printf("\033[33m%-31s\t","Warn");
}
void ErrorLighten(void){
    printf("\033[35m%-31s\t","Error");
}
void CrashLighten(void){
    printf("\033[31m%-31s\t","Crash");
}

static void TerminalLog(struct LogEvent* p_event){
    LogEnlighten level_enlightens[5];
    level_enlightens[0] = &DebugLighten;
    level_enlightens[1] = &InformLighten;
    level_enlightens[2] = &WarnLighten;
    level_enlightens[3] = &ErrorLighten;
    level_enlightens[4] = &ErrorLighten;
    assert(p_event->Level!=Log_Unknow);       
    int index = p_event->Level;
    level_enlightens[index]();
    #ifdef DEBUG_MODE
    printf("%-15s\t%-15s\t%-15d\t",p_event->FileName,p_event->FuncName,p_event->LineNumber);
    #endif
    printf("%-15s\t%-15s\t%-15s\t%-31s\t%-31s\t",p_event->SpaceName,p_event->ProcessName,p_event->ThreadName,p_event->TimeItem,p_event->EventContent);
    printf("\033[0m\n");
}
#define PathLength 64
#define FileNameLength 16
static void SystemFileLog(struct LogEvent* p_event){    
    char DirectoryPath[PathLength];
    getcwd(DirectoryPath,PathLength);
    char FileName[FileNameLength] = "log.txt";    
    char LogFilePath[PathLength];
    sprintf(LogFilePath,"%s%s%s",DirectoryPath,"/",FileName);
    FILE* file = fopen(LogFilePath,"a+");
    assert(file);
    #ifdef DEBUG_MODE
    fprintf(file,"%-15s\t%-15s\t%-15d\t",p_event->FileName,p_event->FuncName,p_event->LineNumber);
    #endif
    fprintf(file,"%-15s\t%-15s\t%-15s\t%-31s\t%-31s\t",p_event->SpaceName,p_event->ProcessName,p_event->ThreadName,p_event->TimeItem,p_event->EventContent);
    fprintf(file,"\033[0m\n");
    fclose(file);    
}

void LogLogger(struct Logger* p_logger,struct LogEvent* p_event){
    assert(p_logger);
    assert(p_event);
    if(p_event->Level<p_logger->m_level)
        return;
    LogAppenderWay appender_ways[2];
    appender_ways[0]=&TerminalLog;
    appender_ways[1]=&SystemFileLog;
    int index = p_logger->m_outputflag;
    (appender_ways[index])(p_event);
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

void RuinLogger(struct Logger* p_logger){
    assert(p_logger);
    if(p_logger->m_eventnum){
        unsigned int index;
        for(index = 0;index < p_logger->m_eventnum;index++){
            free(p_logger->m_events[index]);
            p_logger->m_events[index] = NULL;
        }            
    }
    p_logger->m_eventnum = 0;
    p_logger->m_level = Log_Unknow;
    p_logger->m_outputflag = Unspecified;    
}