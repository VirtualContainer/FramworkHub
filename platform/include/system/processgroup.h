#ifndef PROCESSGROUP_H
#define PROCESSGROUP_H
#include"link.h"

#define MaxProcessNum 8


enum ProcessState
{
    Init=0,
    Ready,
    Running,
    Sleeping,
    Recycled
};

typedef struct Process
{
    char ProcessName[16];
    unsigned int ProcessId;
    char Cmd[64];
    char BinPath[64];
    enum ProcessState State;
    int Fd[2];
    Node_t Node;
}Process_t;

#define MaxResourceItem 8
struct SystemResource
{
    unsigned int CpuQuota;
    unsigned int MemorySize;
    struct
    {
        unsigned int ReadBandwidth;
        unsigned int WriteBandwidth;
    }   IOBandwidth;
    char ExtendResource[0];
}__attribute__((packed));
typedef struct SystemResource Resource_t;

typedef struct ProcessGroup
{
    char m_Name[16];
    unsigned int m_Id;
    unsigned int m_ProcessNum;
    struct Process* m_RootProcess;
    struct SystemResource m_SystemResource;
    Linker_t m_Link; 
}PG_t;


extern void CreatePG(const char* name,struct SystemResource* quota);
extern void SetRootProcess(struct ProcessGroup* group,const char* name);
extern void SetGroupResource(struct ProcessGroup* group,unsigned int cpu,unsigned int memory,unsigned int io);
extern struct Process* GetCurrentProcess(struct ProcessGroup* group);
extern void JoinPG(struct ProcessGroup* group,const char* name,const char* path,const char* cmd);
extern void ExitPG(struct ProcessGroup* group,struct Process* process);
extern bool IsLinearPG(struct Process* process1,struct Process* process2);
#endif