#ifndef UDP_CLSS_H
#define UDP_CLSS_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h> // Sokets libs
#include <netinet/in.h> // Internet Protocol Family Header
#include <arpa/inet.h> // Definitions for internet operations

#include <sys/epoll.h> // Epoll Header
#include <fcntl.h>     // File Control Header



#define MAX_EVENTS 10


#pragma pack(push, 1)
struct protocol_command_udp {
    uint8_t command;
    size_t quantity_byte;
    uint32_t data_client;
};

#pragma pack(pop)


class udp_clss
{
public:
    udp_clss(uint16_t potr_udp, char *ip_addr);
    ~udp_clss();
    int udp_sock;
    int epoll_fd;
    struct sockaddr_in serv_addr;
    socklen_t client_len;
    struct epoll_event ev, events_find[MAX_EVENTS];

};


int set_nonblocking(int fd);
void control_interes_epoll(struct epoll_event *ev, int epoll_fds, int interes_sock, uint32_t interes_event, int mode_ctl);

#endif // UDP_CLSS_H
