#ifndef IPC_H
#define IPC_H

#define _FROM_
#define _TO_

struct Process;

enum IPCWay
{
    MessageQueue,
    SharedMemory,
    Pipe,
    SocketFile
};

#define MaxComunicaterNum 1024
#define MaxInteractPair 1024

typedef struct IPC
{
    struct Process* m_processes[MaxComunicaterNum];
    unsigned m_actives;
    struct 
    {
        enum IPCWay Way;
        _FROM_ struct Process* SourceProcess;
        _TO_ struct Process* ToProcess;
        int (*m_read)(char*);
        int (*m_write)(const char*);
    } m_interact[MaxInteractPair];
    unsigned m_pairs;
}IPC;

extern void InitIPC(IPC* ipc);
extern void JoinIPC(IPC* ipc,struct Process*);
//extern void ExitIPC(IPC* ipc);
extern void ExecIPC(IPC* ipc);
extern void RecycleIPC(IPC* ipc);

#endif