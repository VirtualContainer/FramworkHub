#include "nic.h"


#ifdef DEBUG_MODE 
    #include "init.h"
    #define nic_exception(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Error,"net space","root","root",time_item,message,false);\
    }
    #define nic_debug(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Debug,"net space","root","root",time_item,message,false);\
    }
    #define nic_warn(message)\
    {\
        char time_item[ItemLen];\
        memset(time_item,0,ItemLen);\
        GetFormatDate(time_item,ItemLen,NULL);\
        Logger_t* logger_addr = &g_logger;\
        Log(logger_addr,Log_Warn,"net space","root","root",time_item,message,false);\
    }  
#endif

typedef struct net_interface
{
    const char* ip;
    const unsigned char* mac;
}net_interface_t;

int find_nic(const char* ip, char* name_buf) 
{
    struct in_addr dest_ip;
    inet_pton(AF_INET, ip, &dest_ip);
    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_if_t* pcap_if_list = NULL;
    int err = pcap_findalldevs(&pcap_if_list, err_buf);
    if (err < 0) 
    {
        #ifdef DEBUG_MODE
            nic_exception("Find Failed");
        #endif
        pcap_freealldevs(pcap_if_list);
        return -1;
    }
    pcap_if_t* item;
    for (item = pcap_if_list; item != NULL; item = item->next) 
    {
        if (item->addresses == NULL) {
            continue;
        }
        for (struct pcap_addr* pcap_addr = item->addresses; pcap_addr != NULL; pcap_addr = pcap_addr->next)
        {
            struct sockaddr* sock_addr = pcap_addr->addr;
            if (sock_addr->sa_family != AF_INET) 
                continue;
            struct sockaddr_in* curr_addr = ((struct sockaddr_in*)sock_addr);
            if (curr_addr->sin_addr.s_addr == dest_ip.s_addr) 
            {
                strcpy(name_buf, item->name);
                pcap_freealldevs(pcap_if_list);
                #ifdef DEBUG_MODE
                    nic_debug("NIC Found");
                #endif
                return 0;
            }
        }
    }
    #ifdef DEBUG_MODE
        nic_exception("Empty Found");
    #endif
    pcap_freealldevs(pcap_if_list);
    return -1;
}

int show_nic(void) 
{
    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_if_t* pcapif_list = NULL;
    int count = 0;
    int err = pcap_findalldevs(&pcapif_list, err_buf);
    if (err < 0) 
    {
        #ifdef DEBUG_MODE
            nic_exception("Scan Failed");
        #endif
        fprintf(stderr, "scan net card failed: %s\n", err_buf);
        pcap_freealldevs(pcapif_list);
        return -1;
    }

    #ifdef DEBUG_MODE
        nic_debug("NIC List");
    #endif
    // 遍历所有的可用接口，输出其信息
    for (pcap_if_t* item = pcapif_list; item != NULL; item = item->next) {
        if (item->addresses == NULL) {
            continue;
        }
        for (struct pcap_addr* pcap_addr = item->addresses; pcap_addr != NULL; pcap_addr = pcap_addr->next) {
            char str[INET_ADDRSTRLEN];
            struct sockaddr_in* ip_addr;

            struct sockaddr* sockaddr = pcap_addr->addr;
            if (sockaddr->sa_family != AF_INET) {
                continue;
            }

            ip_addr = (struct sockaddr_in*)sockaddr;
            char * name = item->description;
            if (name == NULL) {
                name = item->name;
            }
            printf("%d: IP:%s name: %s, \n",
                count++,
                name ? name : "unknown",
                inet_ntop(AF_INET, &ip_addr->sin_addr, str, sizeof(str))
            );
            break;
        }
    }
    pcap_freealldevs(pcapif_list);
    if ((pcapif_list == NULL) || (count == 0)) {
        #ifdef DEBUG_MODE
            nic_warn("Empty Found");
        #endif
        return -1;
    }
    return 0;
}

pcap_t * open_nic(const char* ip, const uint8_t* mac_addr) 
{
    char name_buf[256];
    if (find_nic(ip, name_buf) < 0) {
        #ifdef DEBUG_MODE
            show_nic();
        #endif
        return (pcap_t*)0;
    }

    // 根据名称获取ip地址、掩码信息
    char err_buf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    if (pcap_lookupnet(name_buf, &net, &mask, err_buf) == -1) {      
        net = 0;
        mask = 0;
        goto error;
    }

    // 打开设备
    pcap_t * pcap = pcap_create(name_buf, err_buf);
    if (pcap == NULL) {
        #ifdef DEBUG_MODE
            nic_exception("Create Failed");
            show_nic();
        #endif
        return (pcap_t*)0;
    }

    if (pcap_set_snaplen(pcap, 65536) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    if (pcap_set_promisc(pcap, 1) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    if (pcap_set_timeout(pcap, 0) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    // 非阻塞模式读取，程序中使用查询的方式读
    if (pcap_set_immediate_mode(pcap, 1) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    if (pcap_activate(pcap) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    if (pcap_setnonblock(pcap, 0, err_buf) != 0) {
        #ifdef DEBUG_MODE
            nic_exception("Setting Failed");
        #endif
        return (pcap_t*)0;
    }

    // 只捕获发往本接口与广播的数据帧
    char filter_exp[256];
    struct bpf_program fp;
    sprintf(filter_exp,
        "(ether dst %02x:%02x:%02x:%02x:%02x:%02x or ether broadcast) and (not ether src %02x:%02x:%02x:%02x:%02x:%02x)",
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5],
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    if (pcap_compile(pcap, &fp, filter_exp, 0, net) == -1) {
        #ifdef DEBUG_MODE
            nic_exception("Parse Failed");
        #endif
        return (pcap_t*)0;
    }
    if (pcap_setfilter(pcap, &fp) == -1) 
    {
        #ifdef DEBUG_MODE
            nic_exception("Install Failed");
        #endif
        return (pcap_t*)0;
    }
    return pcap;
error:
    #ifdef DEBUG_MODE
        nic_exception("Empty Found");
        show_nic();
    #endif
    return (pcap_t*)0;    
}
