#include "udp_clss.h"

udp_clss::udp_clss() {
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);  // Используем SOCK_DGRAM для UDP [[8]]
    status = udp_sock;
    if (status < 0) printf("Socket creation failed\n");
    else printf("Successfully: UDP socket create\n");

    printf("Successfully: Setting ipv4 ip address\n");

    status = set_nonblocking(udp_sock);
    // Установка сокета в неблокирующий режим
    if (status == -1) close(udp_sock);
    else printf("Successfully: Setting non block socket udp\n");


    epoll_fd = epoll_create1(0);
    status = epoll_fd;
    if (status == -1) perror("epoll_create1");
    else printf("Successfully: Create epoll fd\n");

    status = control_interes_epoll(&ev, epoll_fd, udp_sock, EPOLLIN, EPOLL_CTL_ADD);

    if (status == -1) printf("Erorr table interes\n");
    else printf("Successfully: Setting table interes add\n");
}






udp_clss::~udp_clss() {
    close(udp_sock);
    close(epoll_fd);
}


void udp_clss::set_ip_addr(uint16_t potr_udp, char *ip_addr) {
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(potr_udp);
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
}


int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }
    return 0;
}


int control_interes_epoll(struct epoll_event *ev, int epoll_fds, int interes_sock, uint32_t interes_event, int mode_ctl) {
    ev->events = interes_event; //EPOLLIN; // Интересуемся событием "готов к чтению"
    ev->data.fd = interes_sock; // Связываем событие с дескриптором
    if (epoll_ctl(epoll_fds, mode_ctl, interes_sock, ev) == -1) {
        perror("epoll_ctl: listen_sock");
        close(interes_sock);
        close(epoll_fds);
        return -1;
    }
    return 0;
}
