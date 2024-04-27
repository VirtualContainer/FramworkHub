#ifndef NIC_H
#define NIC_H
#include "pcap.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

extern int find_nic(const char* ip,char* name_buf);
extern int show_nic(void);
extern pcap_t* open_nic(const char* ip, const uint8_t* mac_addr); 
extern void close_nic(pcap_t* nic);


#endif