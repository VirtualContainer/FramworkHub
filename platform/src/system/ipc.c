#include"ipc.h"
#include"processgroup.h"
#include"log.h"
#include<unistd.h>
#include<string.h>

static inline void Pipe_Write(const int fd[],const char content[])
{
    close(fd[0]); 
    int value = write(fd[1],content,strlen(content));
    if(value==-1)
        return;
}

static inline void Pipe_Read(const int fd[],char content[])
{
    close(fd[1]);
    int value = read(fd[0],content,128);
    if(value==-1)
        return;
}

static void Pipe_Communicate()
{

}

static inline void Message_Write(const char content[])
{

}

static inline void Message_Read()
{

}

void InitIPC(IPC* ipc)
{

}

void JoinIPC(IPC* ipc,struct Process* process)
{

}

void ExecIPC(IPC* ipc)
{

}

void RecycleIPC(IPC* ipc)
{
    
}