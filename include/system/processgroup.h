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

struct Process
{
    char ProcessName[16];
    unsigned int ProcessId;
    char BinPath[64];
    enum ProcessState State;
    linker Link; 
};

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

struct ProcessGroup
{
    char m_Name[16];
    unsigned int m_Id;
    unsigned int m_ProcessNum;
    struct Process m_RootProcess;
    struct SystemResource m_SystemResource;
};

extern void CreateProcessGroup(struct ProcessGroup* group,const char* name,struct SystemResource* quota);
extern void SetRootProcess(struct ProcessGroup* group,const char* name);
extern void SetGroupResource(struct ProcessGroup* group,unsigned int cpu,unsigned int memory,unsigned int io);
extern void GetCurrentProcess(struct Process* current);
extern void AddProcess(struct ProcessGroup* group);
extern void ExitProcessGroup(struct ProcessGroup* group);

#endif