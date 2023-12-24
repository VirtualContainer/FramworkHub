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
    char Cmd[64];
    char BinPath[64];
    enum ProcessState State;
    linklist_node Node;
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
    struct Process* m_RootProcess;
    struct SystemResource m_SystemResource;
    linker m_Link; 
};


extern void CreateProcessGroup(const char* name,struct SystemResource* quota);
extern void SetRootProcess(struct ProcessGroup* group,const char* name);
extern void SetGroupResource(struct ProcessGroup* group,unsigned int cpu,unsigned int memory,unsigned int io);
extern struct Process* GetCurrentProcess(struct ProcessGroup* group);
extern void JoinProcessGroup(struct ProcessGroup* group,const char* name,const char* path,const char* cmd);
extern void ExitProcessGroup(struct ProcessGroup* group);

#endif