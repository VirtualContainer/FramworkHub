#ifndef PACKET_H
#define PACKET_H

#define MAX_PACKETSIZE 1516

typedef struct Packet
{
    unsigned size;
    char* data;
    char payload[MAX_PACKETSIZE];
}Packet_t;

extern Packet_t* SendPacket(const unsigned data_size);
extern Packet_t* ReceivePacket(const unsigned data_size);

#endif