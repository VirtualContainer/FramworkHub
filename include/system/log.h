#ifndef LOG_H
#define LOG_H


#define StringLength 32

enum LogLevel{
    Log_Debug = 0,
    Log_Inform,
    Log_Warn,
    Log_Error,
    Log_Crash,
    Log_Unknow
};

struct LogEvent{
    enum LogLevel Level;
    #ifdef DEBUG_MODE
    char FileName[StringLength];
    char FuncName[StringLength];
    unsigned int LineNumber;
    #endif
    char SpaceName[StringLength];
    char ProcessName[StringLength];
    char ThreadName[StringLength];
    char TimeItem[StringLength];
    char EventContent[StringLength];    
};

enum LogOutputFlag{
    Terminal = 0,
    File,
    Unspecified
};

#define LogEventNum 1024
struct Logger{
    struct LogEvent* m_events[LogEventNum];
    unsigned int     m_eventnum;
    enum LogLevel    m_level;
    enum LogOutputFlag m_outputflag;
};

extern void InitLogger(struct Logger*,enum LogLevel,enum LogOutputFlag);
extern void LogLogger(struct Logger*,struct LogEvent*);
extern void RuinLogger(struct Logger*);

#endif