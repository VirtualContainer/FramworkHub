#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>

typedef void (*Task)(void);

void damon(Task task,const char* dir)
{
    pid_t process;
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGHUP,SIG_IGN);
    process = fork();
    if(process)
       exit(0);
    else if(process==-1)
       exit(1);   
    else if(!process)
    {
        setsid();//产生新的会话，并且与父进程会话脱离
        process = fork();
        if(process)
           exit(0);
        else if(process==-1)
           exit(1);
        for(int i=0;i<1024;i++)
           close(i);     
        chdir("/");//改变工作目录
        umask(0);
        signal(SIGCHLD,SIG_IGN);        
        task();  
    } 
}