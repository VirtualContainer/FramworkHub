#include"log.h"
#include<stdio.h>
#include<string.h>

#define node_mask 0x1

int main(int argc,char* argv[]){
    #ifdef DEBUG_MODE
      printf("debug mode...\n");
    #else
      printf("normal mode...\n");
    #endif
    struct Logger logger;
    InitLogger(&logger,Log_Debug,Terminal);
    struct Logger* logger_addr = &logger;
    Log(logger_addr,Log_Debug,"unknown","root","root","unknown","start");
    return 0;
}