#include "packet.h"
#include "pcap.h"

static Packet_t tx_packet;
static Packet_t rx_packet;

Packet_t* SendPacket(const unsigned data_size)
{
    tx_packet.data = tx_packet.payload+MAX_PACKETSIZE-data_size;
    tx_packet.size = data_size;
    return &tx_packet;
}

Packet_t* ReceivePacket(const unsigned data_size)
{
    rx_packet.data = rx_packet.payload;
    return &rx_packet;
}

static void AddHeader(Packet_t* packet,const unsigned header_size)
{
    packet->data -= header_size;
    packet->size += header_size;
}

static void RemoveHeader(Packet_t* packet,const unsigned header_size)
{
    packet->data += header_size;
    packet->size -= header_size;
}


