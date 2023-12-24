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
    char dir[64]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_CPU,GroupName); \
    dir; \
}\
)
#define USER_MEMORY(GroupName) \
(\
{\
    char dir[64]; \
    snprintf(dir,sizeof(dir),"%s%s",SYSTEM_MEMORY,GroupName); \
    dir; \
}\
)
#define USER_IO(GroupName) \
(\
{\
    char dir[64]; \
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

typedef bool (*ResourceConfig)(const char*,const unsigned int);

static struct Process* LocalRootProcess = NULL;
static ResourceConfig SystemConfigs[MaxResourceItem];

#define CreateShareZone(share_type,share_obj,key)\
(\
{\
    static_assert(__builtin_types_compatible_p(typeof(*share_obj), share_type), "TypeMismatch");\
    int share_id = shmget(key,sizeof(share_type),IPC_CREAT|0660);\
    if(share_id == ERROR)\
        share_obj = NULL;\
    else\
        share_obj = (share_type*)shmat(share_id,NULL,0);\
    share_obj;\
}\
)

static struct Process* LoadRootProcess()
{   
    if(!LocalRootProcess)
    {
        key_t key = ftok(".",getpid());
        CreateShareZone(struct Process,LocalRootProcess,key);
    }
    return LocalRootProcess;    
}

static bool InitProcessGroup(struct ProcessGroup* group)
{
    int value = mkdir(USER_CPU(group->m_Name),755);
    if(value)
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

static bool SetSystemResource(struct ProcessGroup* group,struct SystemResource* resource)
{
    char* cpu_dir = USER_CPU(group->m_Name);
    char* memory_dir = USER_MEMORY(group->m_Name);
    char* readstream_dir = USER_IO(group->m_Name);
    char* writestream_dir = USER_IO(group->m_Name);
    char* config_dirs[] = {cpu_dir,memory_dir,readstream_dir,writestream_dir};
    unsigned int* data_list = &resource->CpuQuota;
   
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

static bool InitRootProcess(struct ProcessGroup* group,struct Process* root)
{
    strcpy(root->ProcessName,"Root");
    char* current_path = getcwd(NULL,0);
    strcpy(root->BinPath,current_path);
    root->ProcessId = (unsigned int)getpid();
    root->State = Init;
    InitLinklist(&group->m_Link);
    char task_dir[64];
    snprintf(task_dir,sizeof(task_dir),"%s/tasks",USER_CPU(group->m_Name));
    FILE* task = fopen(task_dir,"w");
    if(!task)
        return false;
    fprintf(task,"%u",root->ProcessId);    
    fclose(task);
    task = NULL;
    group->m_RootProcess->Node.data = (int)getpid();
    AddLinklistnodeTail(&group->m_Link,&(group->m_RootProcess->Node));  
    return true;
}


void SetGroupResource(struct ProcessGroup* group,unsigned int cpu,unsigned int memory,unsigned int io)
{
    group->m_SystemResource.CpuQuota = cpu;
    group->m_SystemResource.IOBandwidth.ReadBandwidth = io;
    group->m_SystemResource.IOBandwidth.WriteBandwidth = io;
    group->m_SystemResource.MemorySize = memory;
    SetSystemResource(group,&group->m_SystemResource);
}

void CreateProcessGroup(const char* name,struct SystemResource* quota)
{
    if(!LoadRootProcess())
        return;
    struct ProcessGroup* group;
    group->m_RootProcess = LoadRootProcess();
    InitRootProcess(group,group->m_RootProcess);
    key_t key = ftok(".",getppid());
    CreateShareZone(typeof(*group),group,key);
    strcpy(group->m_Name,name);
    group->m_Id = (unsigned int)getppid();
    if(!InitProcessGroup(group))
        return;
    group->m_ProcessNum = 1;
}

static struct Process* AddProcess(struct ProcessGroup* group,const char* path,const pid_t id)
{    
    
    struct Process* NewProcess = NULL;
    key_t key = ftok(path,id);
    NewProcess = CreateShareZone(struct Process,NewProcess,key);
    NewProcess->Node.data = id;
    AddLinklistnodeTail(&group->m_Link,&NewProcess->Node);
    NewProcess->State = Init;
    return NewProcess;
}

void JoinProcessGroup(struct ProcessGroup* group,const char* name,const char* path,const char* cmd)
{
    while(getpid()==group->m_RootProcess->ProcessId)
    {
        struct Process* NewProcess;
        int ChildID = fork();
        if(ChildID==ERROR)
            break;
        else if(ChildID==0)
        {
            while(!NewProcess);                
            /*执行子进程的任务*/
            system(NewProcess->Cmd);
        }
        else
        {
            NewProcess = AddProcess(group,path,ChildID);
            NewProcess->State = Ready;
            NewProcess->ProcessId = ChildID;
            strcpy(NewProcess->ProcessName,name);
            strcpy(NewProcess->BinPath,path);
            strcpy(NewProcess->Cmd,cmd);
            #ifdef EnableTraced
               ptrace(PTRACE_ATTACH,ChildID,NULL,NULL); 
            #endif
        }
        break;    
    }
}

void ExitProcessGroup(struct ProcessGroup* group)
{
    
}

struct Process* GetCurrentProcess(struct ProcessGroup* group)
{
    if(!LocalRootProcess)
        return NULL;
    const int key = getpid();
    linklist_node* target;
    SearchLinklistnode(&group->m_Link,&target,key);
    return LinkedHost(struct Process,Node,target);        
}