#include "nic.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
    char name_buf[16];
    memset(name_buf,0,sizeof(name_buf));
    int status = find_nic("192.168.80.131",name_buf);
    printf("Local NIC: %s\n",name_buf);
    show_nic();
    pcap_t* VIF = open_nic("192.168.80.131","00:0c:29:f4:ad:cf");
    int result;
    while(VIF)
    {
        uint8_t buffer[128];
        memset(buffer,0,sizeof(buffer));
        result = pcap_inject(VIF,buffer,sizeof(buffer));       
        sleep(4);
    }
    free(VIF);
    VIF = NULL;    
    return 0;
}