#include"processgroup.h"
#include"log.h"
#include<unistd.h>
#include<sys/ptrace.h>
#include<sys/user.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<string.h>
#include<stdbool.h>


#define SYSTEM_CPU "/sys/fs/cgroup/cpu/"
#define SYSTEM_MEMORY "/sys/fs/cgroup/memory/"
#define SYSTEM_IO "/sys/fs/cgroup/blkio/"

#define USER_CPU(GroupName) \
(\
{\
    char dir[32]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_CPU,GroupName); \
    dir; \
}\
)
#define USER_MEMORY(GroupName) \
(\
{\
    char dir[32]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_MEMORY,GroupName); \
    dir; \
}\
)
#define USER_IO(GroupName) \
(\
{\
    char dir[32]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_IO,GroupName); \
    dir; \
}\
)
#define USER_NET(GroupName) \
(\
{\
    char dir[64]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_NET,GroupName); \
    dir; \
}\
)

static bool InitProcessGroup(struct ProcessGroup* group)
{
    int value = mkdir(USER_CPU(group->m_Name),755);
    if(value)
        return false;
    return true;    
}

static bool InitRootProcess(struct ProcessGroup* group,struct Process* root)
{
    strcpy(root->ProcessName,"Root");
    char* current_path = getcwd(NULL,0);
    strcpy(root->BinPath,current_path);
    root->ProcessId = (unsigned int)getpid();
    root->State = Init;
    InitLinklist(&root->Link);
    char task_dir[64];
    snprintf(task_dir,sizeof(task_dir),"%s/tasks",USER_CPU(group->m_Name));
    FILE* task = fopen(task_dir,"w");
    if(!task)
        return false;
    fprintf(task,"%u",root->ProcessId);    
    fclose(task);
    task = NULL;
    /*设置当前root进程可以被追踪*/
    long value = ptrace(PTRACE_TRACEME,0,NULL,NULL);
    if(value==ERROR)
        return false;    
    return true;
}


static bool CPUConfig(const char* dir,const unsigned int quota)
{
    if(quota==0)
        return true;
    char cpu_dir[64];
    snprintf(cpu_dir,sizeof(cpu_dir),"%s/cpu.cfs_quota_us",dir);    
    FILE* cpu = fopen(cpu_dir,"w");
    if(!cpu)
        return false;
    fprintf(cpu,"%u",quota);
    fclose(cpu);
    cpu = NULL;
    return true;
}
static bool MemoryConfig(const char* dir,const unsigned int size)
{
    if(size==0)
        return true;
    char memory_dir[64];
    snprintf(memory_dir,sizeof(memory_dir),"%s/memory.limit_in_bytes",dir);
    FILE* memory = fopen(memory_dir,"w");
    if(!memory)
        return false;
    fprintf(memory,"%u",size);
    fclose(memory);
    memory = NULL;
    return true;
}
static bool ReadStreamConfig(const char* dir,const unsigned int bandwidth)
{
    if(bandwidth==0)
        return true;
    char in_dir[64];
    snprintf(in_dir,sizeof(in_dir),"%s/blkio.throttle.read_bps_device",dir);    
    FILE* io = fopen(in_dir,"w");
    if(!io)
        return false;
    fprintf(io,"%u",bandwidth);
    fclose(io);
    io = NULL;
    return true;    
}
static bool WriteStreamConfig(const char* dir,const unsigned int bandwidth)
{
    if(bandwidth==0)
        return true;
    char out_dir[64];
    snprintf(out_dir,sizeof(out_dir),"%s/blkio.throttle.write_bps_device",dir);    
    FILE* io = fopen(out_dir,"w");
    if(!io)
        return false;
    fprintf(io,"%u",bandwidth);
    fclose(io);
    io = NULL;
    return true;    
}

typedef bool (*ResourceConfig)(const char*,const unsigned int);
static bool SetSystemResource(struct ProcessGroup* group,struct SystemResource* resource)
{
    char* cpu_dir = USER_CPU(group->m_Name);
    char* memory_dir = USER_MEMORY(group->m_Name);
    char* readstream_dir = USER_IO(group->m_Name);
    char* writestream_dir = USER_IO(group->m_Name);
    char* config_dirs[] = {cpu_dir,memory_dir,readstream_dir,writestream_dir};
    unsigned int* data_list = &resource->CpuQuota;
    ResourceConfig SystemConfigs[MaxResourceItem];
    int index;
    for(index=0;index<MaxResourceItem;index++)
        SystemConfigs[index] = NULL;
    SystemConfigs[0] = &CPUConfig;
    SystemConfigs[1] = &MemoryConfig;
    SystemConfigs[2] = &ReadStreamConfig;
    SystemConfigs[3] = &WriteStreamConfig;
    index = 0;
    bool value = true;
    while(index<MaxResourceItem)
    {
        if(SystemConfigs[index]==NULL)
            break;
        value &= SystemConfigs[index](config_dirs[index],*data_list);
        index++;
        data_list += sizeof(*data_list);
    }
    return value;
}

void CreateProcessGroup(struct ProcessGroup* group,const char* name,struct SystemResource* quota)
{
    strcpy(group->m_Name,name);
    group->m_Id = (unsigned int)getppid();
    if(!InitProcessGroup(group))
        return;
    group->m_ProcessNum = 1;
    InitRootProcess(group,&group->m_RootProcess);
}

void SetGroupResource(struct ProcessGroup* group,unsigned int cpu,unsigned int memory,unsigned int io)
{
    group->m_SystemResource.CpuQuota = cpu;
    group->m_SystemResource.IOBandwidth.ReadBandwidth = io;
    group->m_SystemResource.IOBandwidth.WriteBandwidth = io;
    group->m_SystemResource.MemorySize = memory;
    SetSystemResource(group,&group->m_SystemResource);
}


void AddProcess(struct ProcessGroup* group)
{
    key_t key = ftok(group->m_RootProcess.BinPath,group->m_ProcessNum);    
    struct Process* NewProcess = NULL;
    int share_id;
    share_id = shmget(key,sizeof(struct Process),660);
    if(share_id==ERROR)
        return;
    NewProcess = (struct Process*)shmat(share_id,NULL,0);   

}

void SetCurrentProcess(const char* name)
{
    struct Process* current;
    GetCurrentProcess(current);

}

void GetCurrentProcess(struct Process* current)
{

}